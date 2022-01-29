#pragma once

#include "CoreMinimal.h"
#include <regex>
#include <string>

#include "DataAssets/RpyScript.h"

class RpyParser
{
public:
	FString query;
	std::regex reg_query;
	RpyParser(std::string query) :query(query.c_str()), reg_query(query) {};
	virtual ~RpyParser() = default;
	virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) = 0;
};