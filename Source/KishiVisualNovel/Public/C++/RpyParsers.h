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
    DefineCharacterParser() :RpyParser(4, "^\\$ " + reg_name + " = Character\\(" + reg_string + "(?:, image=" + reg_string + ")?(?:, voice_tag=" + reg_string + ")?\\)$") { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params)
    {
        FName varName = FName(*params[0]);
        FName characterName = FName(*params[1]);
        FName characterImage = FName(*params[2]);
        FName characterVoice = FName(*params[3]);
        FRpyCharacter character = { characterName,characterImage,characterVoice };
        script->characters.Add(varName, character);
        // script->compileData.names.Add(varName, characterName);
        return new BlankInstruction(script, rpyLine);
    };
};
//define audio.sunflower = "music/sun-flower-slow-jam.ogg"
struct DefineAudioParser : public RpyParser {
    DefineAudioParser() :RpyParser(2, "^define audio." + reg_name + " = \"" + reg_path + "\"$") { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params)
    {
        FName name = FName(*params[0]);
        FString path = params[1];
        FRpyAudio audio = { nullptr,path };
        script->audios.Add(name, audio);
        return new BlankInstruction(script, rpyLine);
    };
};
//stop music volume 0.25 fadeout 1.0 fadein 1.0
struct StopAudioParser : public RpyParser {
    StopAudioParser() :RpyParser(6, "^stop " + reg_name + "(?: volume " + reg_ufloatUnit + ")?(?: fadeout " + reg_ufloatUnit + ")?(?: fadein " + reg_ufloatUnit + ")?(?: (loop|noloop))?(?: (if_changed))?$") { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params)
    {
        FName channel = FName(*params[0]);
        FRpyAudioOptions options;
        options.volume = GetFloat(params[1]);
        options.fadeOut = GetFloat(params[2]);
        options.fadeIn = GetFloat(params[3]);
        if(params[4]!="")
        options.loop=params[4]=="loop"?ESchrodBool::ETrue:ESchrodBool::ETrue;
        options.if_changed=params[5]!=""?true:false;
        return new StopInstruction(script, rpyLine, channel, options);
    };
};
//voice "waves.opus"
struct VoiceParser : public RpyParser {
    VoiceParser() :RpyParser(6, "^voice \"" + reg_path + "\"$") { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params)
    {
        FName channel = FName("voice");
        FString& path = params[0];
        FName saveName = FName(*path);
        FRpyAudioOptions options;
        if (!script->audios.Contains(saveName)) {
            FRpyAudio audio = { nullptr,path };
            script->audios.Add(saveName, audio);
        }
        return new PlayInstruction(script, rpyLine, channel, saveName, options);
    };
};

//play music "waves.opus" volume 0.25 fadeout 1.0 fadein 1.0
struct PlayQueueAudioParser : public RpyParser {
    PlayQueueAudioParser() :RpyParser(8, "^(play|queue) " + reg_name + " \"" + reg_path + "\"(?: volume " + reg_ufloatUnit + ")?(?: fadeout " + reg_ufloatUnit + ")?(?: fadein " + reg_ufloatUnit + ")?(?: (loop|noloop))?(?: (if_changed))?$") { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params)
    {
        auto cmd = params[0];
        FName channel = FName(*params[1]);
        FString& path = params[2];
        FName saveName = FName(*path);
        FRpyAudioOptions options;
        options.volume = GetFloat(params[3]);
        options.fadeOut = GetFloat(params[4]);
        options.fadeIn = GetFloat(params[5]);
        if(params[6]!="")
        options.loop=params[6]=="loop"?ESchrodBool::ETrue:ESchrodBool::ETrue;
        options.if_changed=params[7]!=""?true:false;
        if (!script->audios.Contains(saveName)) {
            FRpyAudio audio = { nullptr,path };
            script->audios.Add(saveName, audio);
        }
        if (cmd == "queue")
            return new QueueInstruction(script, rpyLine, channel, saveName, options);
        return new PlayInstruction(script, rpyLine, channel, saveName, options);
    };
};

//play music "waves.opus" volume 0.25 fadeout 1.0 fadein 1.0
struct PlayQueueVarAudioParser : public RpyParser {
    PlayQueueVarAudioParser() :RpyParser(8, "^(play|queue) " + reg_name + " " + reg_name + "(?: volume " + reg_ufloatUnit + ")?(?: fadeout " + reg_ufloatUnit + ")?(?: fadein " + reg_ufloatUnit + ")?(?: (loop|noloop))?(?: (if_changed))?$") { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params)
    {
        auto cmd = params[0];
        FName channel = FName(*params[1]);
        FName keyName = FName(*params[2]);
        FRpyAudioOptions options;
        options.volume = GetFloat(params[3]);
        options.fadeOut = GetFloat(params[4]);
        options.fadeIn = GetFloat(params[5]);
        if(params[6]!="")
        options.loop=params[6]=="loop"?ESchrodBool::ETrue:ESchrodBool::ETrue;
        options.if_changed=params[7]!=""?true:false;
        if (!script->audios.Contains(keyName)) {
            UE_LOG(LogTemp, Error, TEXT("audio name '%s' not found"), (*keyName.ToString()));
            return nullptr;
        }
        if (cmd == "queue")
            return new QueueInstruction(script, rpyLine, channel, keyName, options);
        return new PlayInstruction(script, rpyLine, channel, keyName, options);
    };
};

//"label start:"
struct LabelParser : public RpyParser {
    LabelParser() :RpyParser(1, "^label " + reg_name + ":$") { };
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
    SayParser() :RpyParser(3, "^\"(\\w+)\" " + reg_string + "(?: with (\\w+))?$") { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        FName name = FName(*params[0]);
        FString statement = GetString(params[1]);
        FName with = FName(*params[2]);
        auto say = new SayInstruction(script, rpyLine, name, statement, with);
        return say;
    };
};
struct NarratorSayParser : public RpyParser {
    NarratorSayParser() :RpyParser(2, "^" + reg_string + "(?: with (\\w+))?$") { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        FName name;
        FString statement = GetString(params[0]);
        FName with = FName(*params[1]);
        auto say = new SayInstruction(script, rpyLine, name, statement, with);
        return say;
    };
};

struct CharacterSayParser : public RpyParser {
    CharacterSayParser() :RpyParser(5, "^(\\w+)(?: " + reg_image_name + ")?(?: @ " + reg_image_name + ")? " + reg_string + "(?: with (\\w+))?$") { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        FName name = FName(*params[0]);
        FName image = FName(*params[1]);
        FName atImage = FName(*params[2]);
        FString statement = GetString(params[3]);
        FName with = FName(*params[4]);
        if (script->characters.Contains(name)) {
            name = script->characters[name].name;
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
    ImageParser() :RpyParser(2, "^image " + reg_image_name + " = \"" + reg_path + "\"$") { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        FName name = FName(*params[0]);
        FString path = params[1];
        FRpyImage rpyImage;
        rpyImage.name = name;
        rpyImage.path = path;
        auto names = RpyParser::GetNames(params[0]);
        rpyImage.tag = names[0];
        names.RemoveAt(0);
        rpyImage.attributes = names;
        script->images.Add(name, rpyImage);
        return new BlankInstruction(script, rpyLine);
    };
};
struct ShowParser : public RpyParser {
    ShowParser() :RpyParser(3, "^show " + reg_image_name + "(?: at (\\w+))?(?: with (\\w+))?$") { };
    virtual RpyInstruction* GetRpyInstruction(URpyScript* script, FRpyLine* rpyLine, TArray<FString> params) {
        FName name = FName(*params[0]);
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
        FName name = FName(*params[0]);
        FName at = FName(*params[1]);
        FName with = FName(*params[2]);
        if (!script->images.Contains(name))
            return nullptr;
        return new HideInstruction(script, rpyLine, name, at, with);
    };
};
