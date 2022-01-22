// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <regex>
#include <string>

#include "CoreMinimal.h"
#include "C++/RpyParser.h"

/**
 */
//"\"Sylvie\" \"Hi there! how was class?\""
class SayParser : public RpyParser
{
public:
    SayParser();
    virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params) override;
};

//"label start:"
class LabelParser : public RpyParser
{
public:
    LabelParser();
    virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params) override;
};
//"scene bg meadow"
class SceneParser : public RpyParser
{
public:
    SceneParser();
    virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params) override;
};