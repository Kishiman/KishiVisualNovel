
#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"

// jump loop
struct JumpParser : public RpyParser
{
	JumpParser() : RpyParser(1, "^jump (\\w+)" + reg_comment, "JumpParser") {};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		return new JumpInstruction(script, rpyLine, FName(*params[0]));
	};
};
