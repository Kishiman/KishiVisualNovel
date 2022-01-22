#pragma once

#include "CoreMinimal.h"
#include <regex>
#include <string>

#include "DataAssets/RpyScript.h"

class RpyParser
{
public:
	std::regex query;
	RpyParser() = default;
	virtual ~RpyParser() = default;
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params) = 0;
};