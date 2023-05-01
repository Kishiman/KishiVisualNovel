#include "Rpy/RpySession.h"
#include "Rpy/RpyInstructions.h"
#include "Rpy/RpyParsers.h"
#include "Rpy/RpyInterpreter.h"

// Online IDE - Code Editor, parser, Interpreter

TArray<FName> URpySession::GetLabels() const
{
	TArray<FName> out;
	for (auto script : Scripts)
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
	for (auto script : Scripts)
	{
		this->current = script->labels[label];
		if (this->current)
			break;
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
	bool isMenu = !!(current->Type() & RpyInstructionType::Menu);
	if (!isMenu)
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