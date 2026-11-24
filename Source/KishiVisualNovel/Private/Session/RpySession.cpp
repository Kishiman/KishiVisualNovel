#include "Session/RpySession.h"
#include "Script/RpyInstructions.h"
#include "Script/RpyInstruction.h"
#include "Player/RpyScriptInterpreter.h"
#include "Settings/KishiVisualNovelSettings.h"

void URpySession::PostInitProperties()
{
	Super::PostInitProperties();
}
void URpySession::AddScript(URpyScript *script)
{
	this->scripts.Add(script);
}
void URpySession::ClearScript(URpyScript *script)
{
	scripts.Remove(script);
}
TArray<FName> URpySession::GetLabels() const
{
	TArray<FName> out;
	for (auto script : scripts)
	{
		script->labels.GetKeys(out);
	}
	return out;
};
EInstructionRunTimeType URpySession::GetRunTimeType() const
{
	if (this->current)
		return this->current->RunTimeType();
	return EInstructionRunTimeType::NONE;
}

bool URpySession::StartLabel(FName label)
{
	this->current = nullptr;
	for (auto script : scripts)
	{
		URpyInstruction **found = script ? script->labels.Find(label) : nullptr;
		if (found && *found)
		{
			this->current = *found;
			FDynamicObject::AssignIfNotExist(this->runtimeData, script->compileData);
			break;
		}
	}
	if (!this->current)
	{
		UE_LOG(LogTemp, Error, TEXT("URpySession::StartLabel: Label '%s' was not found in the session's scripts"), *label.ToString());
		return false;
	}
	return Run();
};
bool URpySession::RunNext()
{
	if (!this->current)
		// will call OnScriptComplete and return false;
		return Run();
	this->current = this->current->GetNext(this);
	return Run();
};
bool URpySession::Run()
{
	while (this->current)
	{

		UE_LOG(LogTemp, Display, TEXT("Executing instruction: %s"), *this->current->ToString());
		this->executing = this->current;
		const ERpyInstructionResult Result = this->current->Execute(this);
		this->executing = nullptr;
		this->current->LastResult = Result;
		if (Result == ERpyInstructionResult::Wait)
			break;
		if (Result == ERpyInstructionResult::Failed)
		{
			const ERpyFailedInstructionPolicy Policy = GetDefault<UKishiVisualNovelSettings>()->FailedInstructionPolicy;
			if (Policy == ERpyFailedInstructionPolicy::LogAndStop)
			{
				UE_LOG(LogTemp, Error, TEXT("URpySession::Run: instruction failed, stopping the script (Failed Instruction Policy = Log and Stop): %s"), *this->current->ToString());
				this->current = nullptr;
				break;
			}
			UE_LOG(LogTemp, Error, TEXT("URpySession::Run: instruction failed, moving on to the next one (Failed Instruction Policy = Log and Advance): %s"), *this->current->ToString());
		}
		this->current = this->current->GetNext(this);
	}
	if (!this->current)
	{
		IRpyScriptInterpreter::Execute_OnScriptComplete(this->interpreter.GetObject());
		OnScriptComplete.Broadcast();
		return false;
	}
	return true;
};
bool URpySession::CompleteInstruction(URpyInstruction *Instruction)
{
	if (!this->current)
		return false;
	if (Instruction && Instruction != this->current)
	{
		// A child of the current block, such as one command of a <together>.
		if (!this->current->CompleteChild(Instruction))
			return false;
	}
	else if (!this->current->IsWaitComplete())
	{
		// The current block waits on several children and the completion did not say which one finished.
		UE_LOG(LogTemp, Warning, TEXT("URpySession::CompleteInstruction: a completion for %s did not name its instruction; completing its oldest waiting command. Report completion for the instruction GetExecutingInstruction returned when the operation started."),
			   *this->current->ToString());
		if (!this->current->CompleteFirstChild())
			return false;
	}
	if (this->current->LastResult != ERpyInstructionResult::Wait || !this->current->IsWaitComplete())
		return true;
	UObject *InterpreterObject = this->interpreter.GetObject();
	if (IsValid(InterpreterObject) && InterpreterObject->GetClass()->ImplementsInterface(URpyScriptInterpreter::StaticClass()))
		IRpyScriptInterpreter::Execute_HandleInstructionComplete(InterpreterObject);
	else
		RunNext();
	return true;
}

void URpySession::Skip()
{
	if (this->current && this->current->LastResult == ERpyInstructionResult::Wait)
		this->current->Skip(this);
}

bool URpySession::OnChoice(int index)
{
	if (!current)
		return false;
	if (!current->isOfType(RpyInstructionType::Menu))
	{
		UE_LOG(LogTemp, Error, TEXT("Current is not UMenuInstruction"));
		return false;
	}
	UMenuInstruction *menu = (UMenuInstruction *)(this->current);
	if (index >= menu->choices.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Choice is not in choices range"));
		return false;
	}
	menu->selected = menu->choices[index];
	this->choiceState.choices.Empty();
	return RunNext();
}

URpyState *URpySession::SaveToState_Implementation(UObject *Outer)
{
	URpySessionState *result = Outer ? NewObject<URpySessionState>(Outer)
																	 : NewObject<URpySessionState>(this);
	// Save the current state of the session
	result->currentInstruction = URpyScript::SerializeInstruction(this->current);
	result->instructionsCallStack.Reserve(this->callStack.Num());
	for (auto instruction : this->callStack)
	{
		result->instructionsCallStack.Add(URpyScript::SerializeInstruction(instruction));
	}
	result->scripts.Reserve(this->scripts.Num());
	for (auto script : this->scripts)
	{
		result->scripts.Add(TSoftObjectPtr<URpyScript>(script));
	}
	result->runtimeData = this->runtimeData;
	result->sceneState = this->sceneState;
	result->showState = this->showState;
	result->statementState = this->statementState;
	result->audioState = this->audioState;
	result->choiceState = this->choiceState;

	return result;
}
void URpySession::LoadFromState_Implementation(const URpyState *State)
{
	auto sessionState = Cast<URpySessionState>(State);
	if (!sessionState)
	{
		UE_LOG(LogTemp, Error, TEXT("LoadFromState_Implementation: State is not URpySessionState"));
		return;
	}
	UObject *InterpreterObject = interpreter.GetObject();
	if (!IsValid(InterpreterObject) || !InterpreterObject->GetClass()->ImplementsInterface(URpyScriptInterpreter::StaticClass()))
	{
		UE_LOG(LogTemp, Error, TEXT("LoadFromState_Implementation: Cannot restore the session without a valid script interpreter."));
		return;
	}
	// Load the saved state into the session
	if (sessionState->currentInstruction.script.IsValid() && sessionState->currentInstruction.index != -1)
	{
		this->current = URpyScript::DeserializeInstruction(sessionState->currentInstruction);
	}
	else
	{
		this->current = nullptr;
	}
	this->callStack.Empty();
	for (const auto &instruction : sessionState->instructionsCallStack)
	{
		URpyInstruction *deserialized = URpyScript::DeserializeInstruction(instruction);
		if (deserialized)
		{
			this->callStack.Add(deserialized);
		}
	}
	this->scripts.Empty();
	// load scripts
	for (const auto &scriptPtr : sessionState->scripts)
	{
		if (scriptPtr.IsValid())
		{
			if (!this->scripts.Contains(scriptPtr.Get()))
				this->scripts.Add(scriptPtr.Get());
		}
		else
		{
			auto loadedScript = Cast<URpyScript>(scriptPtr.LoadSynchronous());
			if (loadedScript)
			{
				if (!this->scripts.Contains(loadedScript))
					this->scripts.Add(loadedScript);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("LoadFromState_Implementation: Failed to load script %s"), *scriptPtr.ToString());
			}
		}
	}
	this->runtimeData = sessionState->runtimeData;
	this->sceneState = sessionState->sceneState;
	this->showState = sessionState->showState;
	this->statementState = sessionState->statementState;
	this->audioState = sessionState->audioState;
	this->choiceState = sessionState->choiceState;

	auto audioManager = IRpyScriptInterpreter::Execute_GetAudioManager(interpreter.GetObject());
	auto choiceManager = IRpyScriptInterpreter::Execute_GetChoiceManager(interpreter.GetObject());
	auto sceneManager = IRpyScriptInterpreter::Execute_GetSceneManager(interpreter.GetObject());
	auto showManager = IRpyScriptInterpreter::Execute_GetShowManager(interpreter.GetObject());
	auto statementManager = IRpyScriptInterpreter::Execute_GetStatementManager(interpreter.GetObject());
	if (!IsValid(audioManager.GetObject()) || !IsValid(choiceManager.GetObject()) || !IsValid(sceneManager.GetObject()) ||
		!IsValid(showManager.GetObject()) || !IsValid(statementManager.GetObject()))
	{
		UE_LOG(LogTemp, Error, TEXT("LoadFromState_Implementation: The script interpreter is missing one or more required managers."));
		return;
	}

	IRpyAudioManager::Execute_Reset(audioManager.GetObject());
	IRpyChoiceManager::Execute_Reset(choiceManager.GetObject());
	IRpySceneManager::Execute_Reset(sceneManager.GetObject());
	IRpyShowManager::Execute_Reset(showManager.GetObject());
	IRpyStatementManager::Execute_Reset(statementManager.GetObject());

	for (size_t i = 0; i < sessionState->audioState.audioStates.Num(); i++)
	{
		IRpyAudioManager::Execute_PlayAudio(audioManager.GetObject(), sessionState->audioState.audioStates[i].channel, sessionState->audioState.audioStates[i].rpyAudio, sessionState->audioState.audioStates[i].options);
	}

	if (sessionState->choiceState.choices.Num() > 0)
	{
		IRpyChoiceManager::Execute_Menu(choiceManager.GetObject(), sessionState->choiceState.choices);
	}

	IRpySceneManager::Execute_Scene(sceneManager.GetObject(), sessionState->sceneState.rpyImage, sessionState->sceneState.options);

	for (size_t i = 0; i < sessionState->showState.imageStates.Num(); i++)
	{
		IRpyShowManager::Execute_Show(showManager.GetObject(), sessionState->showState.imageStates[i].rpyImage, sessionState->showState.imageStates[i].options);
	}

	IRpyStatementManager::Execute_Say(statementManager.GetObject(), sessionState->statementState.name, sessionState->statementState.statement);

	// The saved instruction was waiting when the game was saved. A line or a menu waits for the player again. Anything else (a show
	// or hide transition, a pause, a <together>) runs again, since the waits it had started are not part of the save.
	if (this->current)
	{
		if (this->current->IsA<USayInstruction>() || this->current->isOfType(RpyInstructionType::Menu))
			this->current->LastResult = ERpyInstructionResult::Wait;
		else
			Run();
	}
}
