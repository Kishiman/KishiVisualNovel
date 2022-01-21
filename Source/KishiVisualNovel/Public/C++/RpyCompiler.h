#pragma once

#include "CoreMinimal.h"
#include <regex>
#include <string>

#include "DataAssets/RpyScript.h"

class RpyCompiler
{
public:
	std::regex query;
	RpyCompiler() = default;
	virtual ~RpyCompiler() = default;
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine line, TArray<FString> params) = 0;
};