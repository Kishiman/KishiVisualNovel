// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <regex>
#include <string>

#include "CoreMinimal.h"
#include "C++/RpyParser.h"

/**
 */
class SayParser : public RpyParser
{
public:
    SayParser();
    virtual ~SayParser() = default;
    virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params) override;
};
