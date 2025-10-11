#include "Rpy/RpySession.h"
#include "./Rpy/RpyParsers.h"
#include "Rpy/RpyInstruction.h"
#include "Rpy/RpyParser.h"
#include "Interfaces/RpyScriptInterpreter.h"

// Online IDE - Code Editor, parser, Interpreter

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
	scripts.Empty();
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
	for (auto script : scripts)
	{
		this->current = script->labels[label];
		if (this->current)
		{
			FDynamicObject::AssignIfNotExist(this->runtimeData, script->compileData);
			break;
		}
	}
	if (!this->current)
		return true;
	return Run();
};
bool URpySession::RunNext()
{
	if (!this->current)
		return false;
	this->current = this->current->GetNext(this);
	return Run();
};
bool URpySession::Run()
{
	if (!this->current)
		return false;
	while (this->current)
	{

		UE_LOG(LogTemp, Display, TEXT("Executing rpy:%s"), (*this->current->rpyLine->line));
		try
		{
			bool autoNext = this->current->Execute(this);
			if (!autoNext)
				break;
			this->current = this->current->GetNext(this);
		}
		catch (const std::exception &e)
		{
			UE_LOG(LogTemp, Error, TEXT("Caught exception: %s"), e.what());
			return false;
		}
	}
	return true;
};
bool URpySession::OnChoice(int index)
{
	if (!current)
		return false;
	if (!current->isOfType(RpyInstructionType::Menu))
	{
		UE_LOG(LogTemp, Error, TEXT("Current is not MenuInstruction"));
		return false;
	}
	MenuInstruction *menu = (MenuInstruction *)(this->current);
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
																	 : NewObject<URpySessionState>();
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
	// Load the saved state into the session
	this->current = URpyScript::DeserializeInstruction(sessionState->currentInstruction);
	this->callStack.Empty();
	for (const auto &instruction : sessionState->instructionsCallStack)
	{
		RpyInstruction *deserialized = URpyScript::DeserializeInstruction(instruction);
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

	return;
}