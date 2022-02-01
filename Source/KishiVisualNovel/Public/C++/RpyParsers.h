// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <regex>

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "C++/RpyParser.h"
#include "C++/RpyInstructions.h"

/**
 */
 //"init:"
struct InitParser : public RpyParser {
    InitParser() :RpyParser(0, "^init:$") { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params)
    {
        RpyInstruction* init = new BlankInstruction(script, rpyLine);
        script->init = init;
        return init;
    };
};
//"$ e = Character('Eileen')"
struct DefineCharacterParser : public RpyParser {
    DefineCharacterParser() :RpyParser(3, reg_define_char) { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params)
    {
        FName varName = FName(*params[0]);
        FName characterName = FName(*params[1]);
        script->compileData.names.Add(varName, characterName);
        return new BlankInstruction(script, rpyLine);
    };
};

//"label start:"
struct LabelParser : public RpyParser {
    LabelParser() :RpyParser(1, reg_label) { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        RpyInstruction* label = new BlankInstruction(script, rpyLine);
        script->labels.Add(FName(*params[0]), label);
        return label;
    };
};

//"jump start"
struct JumpParser : public RpyParser {
    JumpParser() :RpyParser(1, "^jump (\\w+)$") { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        FName name = FName(*params[0]);
        auto jump = new JumpInstruction(script, rpyLine, name);
        return jump;
    };
};

//"call start"
struct CallParser : public RpyParser {
    CallParser() :RpyParser(1, "^call (\\w+)$") { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        FName name = FName(*params[0]);
        auto call = new CallInstruction(script, rpyLine, name);
        return call;
    };
};

struct SayParser : public RpyParser {
    SayParser() :RpyParser(3, "^\"(\\w+)\" " + RpyParser::reg_string + "(?: with (\\w+))?$") { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        FName name = FName(*params[0]);
        FString statement = GetString(params[1]);
        FName with = FName(*params[2]);
        auto say = new SayInstruction(script, rpyLine, name, statement, with);
        return say;
    };
};
struct NarratorSayParser : public RpyParser {
    NarratorSayParser() :RpyParser(2, "^" + RpyParser::reg_string + "(?: with (\\w+))?$") { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        FName name;
        FString statement = GetString(params[0]);
        FName with = FName(*params[1]);
        auto say = new SayInstruction(script, rpyLine, name, statement, with);
        return say;
    };
};

struct CharacterSayParser : public RpyParser {
    CharacterSayParser() :RpyParser(5, "^(\\w+)(?: " + reg_image_name + ")?(?: @ " + reg_image_name + ")? " + RpyParser::reg_string + "(?: with (\\w+))?$") { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        FName name = FName(*params[0]);
        FName image = FName(*params[1]);
        FName atImage = FName(*params[2]);
        FString statement = GetString(params[3]);
        FName with = FName(*params[4]);
        if (script->compileData.names.Contains(name)) {
            name = script->compileData.names[name];
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("character name '%s' not found"), (*name.ToString()));
            return nullptr;
        }
        auto say = new SayInstruction(script, rpyLine, name, statement, with);
        return say;
    };
};

struct ImageParser : public RpyParser {
    ImageParser() :RpyParser(2, "^image " + reg_image_name + " = \"([/\\w\\.\\s]+)\"$") { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        FName name = FName(*params[0]);
        FString path = params[1];
        FRpyImage rpyImage;
        rpyImage.name=name;
        rpyImage.path=path;
        auto names = RpyParser::GetNames(params[0]);
        rpyImage.tag=names[0];
        names.RemoveAt(0);
        rpyImage.attributes=names;
        script->images.Add(name, rpyImage);
        return new BlankInstruction(script, rpyLine);
    };
};
struct ShowParser : public RpyParser {
    ShowParser() :RpyParser(3, "^show " + reg_image_name + "(?: at (\\w+))?(?: with (\\w+))?$") { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        FName name=FName(*params[0]);
        FName at = FName(*params[1]);
        FName with = FName(*params[2]);
        if (!script->images.Contains(name))
            return nullptr;
        return new ShowInstruction(script, rpyLine, name, at, with);
    };
};
struct HideParser : public RpyParser {
    HideParser() :RpyParser(3, "^hide " + reg_image_name + "(?: at (\\w+))?(?: with (\\w+))?$") { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        FName name=FName(*params[0]);
        FName at = FName(*params[1]);
        FName with = FName(*params[2]);
        if (!script->images.Contains(name))
            return nullptr;
        return new HideInstruction(script, rpyLine, name, at, with);
    };
};
