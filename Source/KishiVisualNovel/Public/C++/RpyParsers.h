// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <regex>
#include <string>

#include "CoreMinimal.h"
#include "C++/RpyParser.h"
#include "C++/RpyInstructions.h"

/**
 */
 //"init:"
class InitParser : public RpyParser {
public:
    InitParser() { query = "^init:$"; };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params)
    {
        RpyInstruction* init = new BlankInstruction(script, rpyLine);
        script->init = init;
        return init;
    };
};
//"$ e = Character('Eileen')"
class DefineCharacterParser : public RpyParser {
public:
    DefineCharacterParser() { query = "^\\$ (\\w+) = Character\\(('\\w+')\\)$"; };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params)
    {
        FName varName = FName(*params[0]);
        FName characterName = FName(*params[1]);
        script->compileData.names.Add(varName, characterName);
        return new BlankInstruction(script, rpyLine);
    };
};

//"label start:"
class LabelParser : public RpyParser {
public:
    LabelParser() { query = "^label (\\w+):$"; };;
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        RpyInstruction* label = new BlankInstruction(script, rpyLine);
        script->labels.Add(FName(*params[0]), label);
        return label;
    };
};


//"\"Sylvie\" \"Hi there! how was class?\""
class SayParser : public RpyParser {
public:
    SayParser() { query = "^(?:(\\w+|\".+\") )?\"(.*)\"$"; };;
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) override;
};