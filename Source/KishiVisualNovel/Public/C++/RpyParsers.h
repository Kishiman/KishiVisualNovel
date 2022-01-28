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
    InitParser() { query = "^init:$"; };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params)
    {
        RpyInstruction* init = new BlankInstruction(script, rpyLine);
        script->init = init;
        return init;
    };
};
//"$ e = Character('Eileen')"
struct DefineCharacterParser : public RpyParser {
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
struct LabelParser : public RpyParser {
    LabelParser() { query = "^label (\\w+):$"; };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        RpyInstruction* label = new BlankInstruction(script, rpyLine);
        script->labels.Add(FName(*params[0]), label);
        return label;
    };
};

//"jump start"
struct JumpParser : public RpyParser {
    JumpParser() { query = "^jump (\\w+)$"; };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        FName name =        FName(*params[0]);
        auto jump=new JumpInstruction(script, rpyLine,name);
        return jump;
    };
};

//"call start"
struct CallParser : public RpyParser {
    CallParser() { query = "^call (\\w+)$"; };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        FName name =        FName(*params[0]);
        auto call=new CallInstruction(script, rpyLine,name);
        return call;
    };
};


//"\"Sylvie\" \"Hi there! how was class?\""
struct SayParser : public RpyParser {
    SayParser() { query = "^(?:((\\w+)|\"(\\w)+\") (?:(\\w+ ))?(?:@ (\\w+ ))?)?\"([{}\\/!? '.\\w]*)\"(?: with (\\w+))?$"; };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        FName name_exp =    FName(*params[1]);
        FName name =        FName(*params[2]);
        FName image =       FName(*params[3]);
        FName atImage =     FName(*params[4]);
        FString statement = params[5];
        FName with =        FName(*params[6]);
        if(name_exp.Len()>0){
            if (script->compileData.names.Contains(name)) {
                name = script->compileData.names[name];
            }
            else { return nullptr; }
        }
        auto say=new SayInstruction(script, rpyLine, name, statement,with);
        return say;
    };
};
struct ImageParser : public RpyParser {
    ImageParser() { query = "^image((\\s\\w+)+) = \"([/\\w\\.\\s]+)\"$"; };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        FName name = FName(*params[0]);
        FString path = params[2];
        UPaperSprite* image = Cast<UPaperSprite>(StaticLoadObject(UPaperSprite::StaticClass(), NULL, *path));
        if (image) {
            script->images.Add(name, image);
            return new BlankInstruction(script, rpyLine);
        }

        return nullptr;
    };
};
struct ShowParser : public RpyParser {
    ShowParser() { query = "^show(( (?!at|with)\\w+)+)(?: at (\\w+))?(?: with (\\w+))?$"; };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        FName name = FName(*params[0]);
        FName at = FName(*params[1]);
        FName with = FName(*params[2]);
        if (!script->images.Contains(name))
            return nullptr;
        return new ShowInstruction(script, rpyLine, name, at, with);
    };
};