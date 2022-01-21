// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <regex>
#include <string>

#include "CoreMinimal.h"
#include "C++/RpyCompiler.h"

/**
 */
class SayCompiler : public RpyCompiler
{
public:
    SayCompiler();
    virtual ~SayCompiler() = default;
    virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine line, TArray<FString> params) override;
};
