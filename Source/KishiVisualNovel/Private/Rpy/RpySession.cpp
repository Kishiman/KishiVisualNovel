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
	return RunNext();
}

FRpyState URpySession::SaveState()
{
	FRpyState result;
	// Save the current state of the session
	result.currentInstruction = URpyScript::SerializeInstruction(this->current);
	result.instructionsCallStack.Reserve(this->callStack.Num());
	for (auto instruction : this->callStack)
	{
		result.instructionsCallStack.Add(URpyScript::SerializeInstruction(instruction));
	}
	result.runtimeData = this->runtimeData;

	auto sceneManager = IRpyScriptInterpreter::Execute_GetSceneManager(interpreter.GetObject());
	auto showManager = IRpyScriptInterpreter::Execute_GetShowManager(interpreter.GetObject());
	auto statementManager = IRpyScriptInterpreter::Execute_GetStatementManager(interpreter.GetObject());
	auto audioManager = IRpyScriptInterpreter::Execute_GetAudioManager(interpreter.GetObject());
	auto choiceManager = IRpyScriptInterpreter::Execute_GetChoiceManager(interpreter.GetObject());

	result.sceneState = IRpySceneManager::Execute_GetCurrentSceneState(sceneManager.GetObject());
	result.showStates = IRpyShowManager::Execute_GetCurrentShowStates(showManager.GetObject());
	result.statementState = IRpyStatementManager::Execute_GetCurrentStatementState(statementManager.GetObject());
	result.audioStates = IRpyAudioManager::Execute_GetCurrentAudioStates(audioManager.GetObject());
	result.choiceState = IRpyChoiceManager::Execute_GetCurrentChoiceState(choiceManager.GetObject());

	return result;
}
void URpySession::LoadState(const FRpyState &State)
{
	// Load the saved state into the session
	URpyScript::DeserializeInstruction(State.currentInstruction);
	this->callStack.Empty();
	for (const auto &instruction : State.instructionsCallStack)
	{
		RpyInstruction *deserialized = URpyScript::DeserializeInstruction(instruction);
		if (deserialized)
		{
			this->callStack.Add(deserialized);
		}
	}
	this->runtimeData = State.runtimeData;

	auto sceneManager = IRpyScriptInterpreter::Execute_GetSceneManager(interpreter.GetObject());
	auto showManager = IRpyScriptInterpreter::Execute_GetShowManager(interpreter.GetObject());
	auto statementManager = IRpyScriptInterpreter::Execute_GetStatementManager(interpreter.GetObject());
	auto audioManager = IRpyScriptInterpreter::Execute_GetAudioManager(interpreter.GetObject());
	auto choiceManager = IRpyScriptInterpreter::Execute_GetChoiceManager(interpreter.GetObject());

	auto sceneState=State.sceneState;
	IRpySceneManager::Execute_Scene(sceneManager.GetObject(), sceneState.rpyImage, sceneState.options);
	auto showStates=State.showStates;
	for (size_t i = 0; i < showStates.Num(); i++)
	{
		IRpyShowManager::Execute_Show(showManager.GetObject(), showStates[i].rpyImage, showStates[i].options);
	}
	auto statementState=State.statementState;
	IRpyStatementManager::Execute_Say(statementManager.GetObject(), statementState.name, statementState.statement);
	auto audioStates=State.audioStates;
	for (size_t i = 0; i < audioStates.Num(); i++)
	{
		IRpyAudioManager::Execute_PlayAudio(audioManager.GetObject(), audioStates[i].channel, audioStates[i].rpyAudio, audioStates[i].options);
	}
	auto choiceState=State.choiceState;
	if(choiceState.choices.Num()>0)
	{
		IRpyChoiceManager::Execute_Menu(choiceManager.GetObject(), choiceState.choices);
	}

}