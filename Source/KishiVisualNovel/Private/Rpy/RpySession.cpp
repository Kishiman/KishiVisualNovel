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
	// result.sceneState = this->sceneState;
	// result.showStates = this->showStates;
	// result.statementState = this->statementState;
	// result.audioStates = this->audioStates;
	// result.choiceState = this->choiceState;

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

	// IRpySceneManager::Execute_Scene(sceneManager.GetObject(), State.sceneState.rpyImage, State.sceneState.options);
	// IRpyShowManager::Execute_ClearAll(showManager.GetObject());
	// for (size_t i = 0; i < State.showStates.Num(); i++)
	// {
	// 	IRpyShowManager::Execute_Show(showManager.GetObject(), State.showStates[i].rpyImage, State.showStates[i].options);
	// }
	// IRpyStatementManager::Execute_Say(statementManager.GetObject(), State.statementState.name, State.statementState.statement);
	// IRpyAudioManager::Execute_ClearAll(audioManager.GetObject());
	// for (size_t i = 0; i < State.audioStates.Num(); i++)
	// {
	// 	IRpyAudioManager::Execute_PlayAudio(audioManager.GetObject(), State.audioStates[i].channel, State.audioStates[i].rpyAudio, State.audioStates[i].options);
	// }
	// if (State.choiceState.choices.Num() > 0)
	// {
	// 	IRpyChoiceManager::Execute_Menu(choiceManager.GetObject(), State.choiceState.choices);
	// }
}