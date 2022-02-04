#include "DataAssets/RpySession.h"
#include "C++/RpyParsers.h"
#include "C++/RpyInstructions.h"
#include "Interfaces/RpyInterpreter.h"
#include "Misc/FileHelper.h" 

// Online IDE - Code Editor, parser, Interpreter


TArray<FName> URpySession::GetLabels() const
{
	TArray<FName> out;
	for(auto script:Scripts){
		script->labels.GetKeys(out);
	}
	return out;
};

bool URpySession::StartLabel(const TScriptInterface<IRpyInterpreter>& interpreter, FName label)
{
	for(auto script:Scripts){
		this->current = script->labels[label];
		if (this->current)
			break;
	}
	if (!this->current)
		return true;
	return Run(interpreter);
};
bool URpySession::RunNext(const TScriptInterface<IRpyInterpreter>& interpreter)
{
	this->current = this->current->GetNext(interpreter);
	return Run(interpreter);
};
bool URpySession::Run(const TScriptInterface<IRpyInterpreter>& interpreter)
{
	while (this->current && IRpyInterpreter::Execute_AutoRunNext(interpreter.GetObject()))
	{
		bool allGood = this->current->Execute(interpreter);
		if (!allGood)
			return false;
		this->current = this->current->GetNext(interpreter);
	}
	return true;
};