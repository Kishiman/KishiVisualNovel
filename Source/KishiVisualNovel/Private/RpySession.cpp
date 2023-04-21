#include "RpySession.h"
#include "C++/RpyInstructions.h"
#include "C++/RpyParsers.h"
#include "Interfaces/RpyInterpreter.h"

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
	bool autoNext = true;
	if (!this->current)
		return false;
	while (this->current)
	{

		bool allGood = this->current->Execute(this, autoNext);
		if (!allGood)
			return false;
		if (!autoNext)
			break;
		this->current = this->current->GetNext(this);
	}
	return true;
};