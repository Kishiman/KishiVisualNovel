#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"

// "Yes":
struct ChoiceParser : public RpyParser
{
	ChoiceParser() : RpyParser(1, "^" + RpyParser::reg_string + ":" + reg_comment, "ChoiceParser") {};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		return new ChoiceInstruction(script, rpyLine, RpyParser::GetString(params[0]));
	};
};