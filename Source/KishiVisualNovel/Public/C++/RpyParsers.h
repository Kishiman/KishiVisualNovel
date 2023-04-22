// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <regex>

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "C++/RpyParser.h"
#include "C++/RpyInstructions.h"
#include <string>

/**
 */
//"init:"
struct InitParser : public RpyParser
{
	InitParser() : RpyParser(0, "^init:$", "InitParser"){};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		RpyInstruction *init = new RpyInstruction(script, rpyLine);
		script->init = init;
		return init;
	};
};
//$ variable_name = "Hello, world!"
struct DefineStringParser : public RpyParser
{
	DefineStringParser() : RpyParser(4, "^(\\$|default) " + reg_name + " = " + reg_string + "$", "DefineStringParser"){};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		bool isDefault = params[0] == "default";
		FName varName = FName(*params[1]);
		FString value = GetString(params[2]);
		if (isDefault)
			script->compileData.strings.Add(varName, value);
		return new AssignInstruction<FString>(script, rpyLine, varName, value);
	};
};
//"$ e = Character('Eileen')"
struct DefineCharacterParser : public RpyParser
{
	DefineCharacterParser() : RpyParser(4, "^\\$ " + reg_name + " = Character\\(" + reg_string + "(?:, image=" + reg_string + ")?(?:, voice_tag=" + reg_string + ")?\\)$", "DefineCharacterParser"){};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FName varName = FName(*params[0]);
		FName characterName = FName(*params[1]);
		FName characterImage = FName(*params[2]);
		FName characterVoice = FName(*params[3]);
		script->characters.Add(varName, {characterName, characterImage, characterVoice});
		// script->compileData.names.Add(varName, characterName);
		return new RpyInstruction(script, rpyLine);
	};
};
// define audio.sunflower = "music/sun-flower-slow-jam.ogg"
struct DefineAudioParser : public RpyParser
{
	DefineAudioParser() : RpyParser(2, "^define audio." + reg_name + " = \"" + reg_path + "\"$", "DefineAudioParser"){};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FName name = FName(*params[0]);
		FString path = params[1];
		FRpyAudio audio = {nullptr, path};
		script->audios.Add(name, audio);
		return new RpyInstruction(script, rpyLine);
	};
};
// stop music volume 0.25 fadeout 1.0 fadein 1.0
struct StopAudioParser : public RpyParser
{
	StopAudioParser() : RpyParser(6, "^stop " + reg_name + "(?: volume " + reg_ufloatUnit + ")?(?: fadeout " + reg_ufloat + ")?(?: fadein " + reg_ufloat + ")?(?: (loop|noloop))?(?: (if_changed))?$", "StopAudioParser"){};

	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FName channel = FName(*params[0]);
		FRpyAudioOptions options;
		options.volume = GetFloat(params[1]);
		options.fadeOut = GetFloat(params[2]);
		options.fadeIn = GetFloat(params[3]);
		if (params[4] != "")
			options.loop = params[4] == "loop" ? ESchrodBool::ETrue : ESchrodBool::ETrue;
		options.if_changed = params[5] != "" ? true : false;
		return new StopInstruction(script, rpyLine, channel, options);
	};
};
// voice "waves.opus"
struct VoiceParser : public RpyParser
{
	VoiceParser() : RpyParser(1, "^voice \"" + reg_path + "\"$", "VoiceParser"){};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FName channel = FName("voice");
		FString &path = params[0];
		FName saveName = FName(*path);
		FRpyAudioOptions options;
		if (!script->audios.Contains(saveName))
		{
			FRpyAudio audio = {nullptr, path};
			script->audios.Add(saveName, audio);
		}
		return new PlayInstruction(script, rpyLine, channel, saveName, options);
	};
};

// play music "waves.opus" volume 0.25 fadeout 1.0 fadein 1.0
struct PlayQueueAudioParser : public RpyParser
{
	PlayQueueAudioParser() : RpyParser(8, "^(play|queue) " + reg_name + " \"" + reg_path + "\"(?: volume " + reg_ufloatUnit + ")?(?: fadeout " + reg_ufloat + ")?(?: fadein " + reg_ufloat + ")?(?: (loop|noloop))?(?: (if_changed))?$", "PlayQueueAudioParser"){};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		auto cmd = params[0];
		FName channel = FName(*params[1]);
		FString &path = params[2];
		FName saveName = FName(*path);
		FRpyAudioOptions options;
		options.volume = GetFloat(params[3]);
		options.fadeOut = GetFloat(params[4]);
		options.fadeIn = GetFloat(params[5]);
		if (params[6] != "")
			options.loop = params[6] == "loop" ? ESchrodBool::ETrue : ESchrodBool::ETrue;
		options.if_changed = params[7] != "" ? true : false;
		if (!script->audios.Contains(saveName))
		{
			FRpyAudio audio = {nullptr, path};
			script->audios.Add(saveName, audio);
		}
		if (cmd == "queue")
			return new QueueInstruction(script, rpyLine, channel, saveName, options);
		return new PlayInstruction(script, rpyLine, channel, saveName, options);
	};
};

// play music track_1 volume 0.25 fadeout 1.0 fadein 1.0
struct PlayQueueVarAudioParser : public RpyParser
{
	PlayQueueVarAudioParser() : RpyParser(8, "^(play|queue) " + reg_name + " " + reg_name + "(?: volume " + reg_ufloatUnit + ")?(?: fadeout " + reg_ufloat + ")?(?: fadein " + reg_ufloat + ")?(?: (loop|noloop))?(?: (if_changed))?$", "PlayQueueVarAudioParser"){};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		auto cmd = params[0];
		FName channel = FName(*params[1]);
		FName keyName = FName(*params[2]);
		FRpyAudioOptions options;
		options.volume = GetFloat(params[3]);
		options.fadeOut = GetFloat(params[4]);
		options.fadeIn = GetFloat(params[5]);
		if (params[6] != "")
			options.loop = params[6] == "loop" ? ESchrodBool::ETrue : ESchrodBool::ETrue;
		options.if_changed = params[7] != "" ? true : false;
		if (!script->audios.Contains(keyName))
		{
			UE_LOG(LogTemp, Error, TEXT("audio name '%s' not found"), (*keyName.ToString()));
			return nullptr;
		}
		if (cmd == "queue")
			return new QueueInstruction(script, rpyLine, channel, keyName, options);
		return new PlayInstruction(script, rpyLine, channel, keyName, options);
	};
};

// label start:
struct LabelParser : public RpyParser
{
	LabelParser() : RpyParser(1, "^label " + reg_name + ":$", "LabelParser"){};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		RpyInstruction *label = new LabelInstruction(script, rpyLine);
		script->labels.Add(FName(*params[0]), label);
		return label;
	};
};

// jump loop
struct JumpParser : public RpyParser
{
	JumpParser() : RpyParser(1, "^jump (\\w+)$", "JumpParser"){};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		return new JumpInstruction(script, rpyLine, FName(*params[0]));
	};
};

//"\"Sylvie\" \"Hi there! how was class?\""
struct SayParser : public RpyParser
{
	SayParser() : RpyParser(3, "^(?:(\\w+|\".+\") )?" + reg_string + "(?: with (\\w+))?$", "SayParser"){};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		bool literal = params[0].StartsWith("\"") && params[0].EndsWith("\"");
		if (literal)
			params[0] = params[0].Mid(1, params[0].Len() - 2);
		FName name = FName(*params[0]);
		FString statement = GetString(params[1]);
		FName with = FName(*params[2]);
		if (!literal)
		{
			if (script->compileData.names.Contains(name))
			{
				name = script->compileData.names[name];
			}
			else
				return nullptr;
		}
		return new SayInstruction(script, rpyLine, name, statement, with);
	};
};
//"\"Hope everything is okay\""
struct SayParser2 : public RpyParser
{
	SayParser2() : RpyParser(2, "^" + reg_string + "(?: with (\\w+))?$", "SayParser2"){};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FName name = FName("");
		FString statement = GetString(params[0]);
		FName with = FName(*params[1]);
		return new SayInstruction(script, rpyLine, name, statement, with);
	};
};

// image = "cool_image"
struct ImageParser : public RpyParser
{
	ImageParser() : RpyParser(2, "^image " + reg_image_name + " = \"" + reg_path + "\"$", "ImageParser"){};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
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
		return new RpyInstruction(script, rpyLine);
	};
};

// show john happy
struct ShowParser : public RpyParser
{
	ShowParser() : RpyParser(3, "^show " + reg_image_name + "(?: at (\\w+))?(?: with (\\w+))?$", "ShowParser"){};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FName name = FName(*params[0]);
		FName at = FName(*params[1]);
		FName with = FName(*params[2]);
		if (!script->images.Contains(name))
			return nullptr;
		return new ShowInstruction(script, rpyLine, name, at, with);
	};
};

// if True :
struct IfBoolParser : public RpyParser
{
	IfBoolParser() : RpyParser(2, "^if (!)?(\\w+|True|False):$", "IfBoolParser"){};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		bool reverse = params[0] == "!";
		if (params[1] == "True")
			return new IfInstruction(script, rpyLine, !reverse);
		if (params[1] == "False")
			return new IfInstruction(script, rpyLine, reverse);
		FName name = FName(*params[1]);
		if (!script->compileData.bools.Contains(name))
		{
			return nullptr;
		}
		return new IfBoolInstruction(script, rpyLine, name, reverse);
	};
};

//"call start"
struct CallParser : public RpyParser
{
	CallParser() : RpyParser(1, "^call (\\w+)$", "CallParser"){};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FName name = FName(*params[0]);
		auto call = new CallInstruction(script, rpyLine, name);
		return call;
	};
};

struct NarratorSayParser : public RpyParser
{
	NarratorSayParser() : RpyParser(2, "^" + reg_string + "(?: with (\\w+))?$", "NarratorSayParser"){};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FName name;
		FString statement = GetString(params[0]);
		FName with = FName(*params[1]);
		auto say = new SayInstruction(script, rpyLine, name, statement, with);
		return say;
	};
};

struct CharacterSayParser : public RpyParser
{
	CharacterSayParser() : RpyParser(5, "^(\\w+)(?: " + reg_image_name + ")?(?: @ " + reg_image_name + ")? " + reg_string + "(?: with (\\w+))?$", "CharacterSayParser"){};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FName name = FName(*params[0]);
		FName image = FName(*params[1]);
		FName atImage = FName(*params[2]);
		FString statement = GetString(params[3]);
		FName with = FName(*params[4]);
		if (script->characters.Contains(name))
		{
			name = script->characters[name].name;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("character name '%s' not found"), (*name.ToString()));
			return nullptr;
		}
		return new SayInstruction(script, rpyLine, name, statement, with);
	};
};

struct HideParser : public RpyParser
{
	HideParser() : RpyParser(3, "^hide " + reg_image_name + "(?: at (\\w+))?(?: with (\\w+))?$", "HideParser"){};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FName name = FName(*params[0]);
		FName at = FName(*params[1]);
		FName with = FName(*params[2]);
		if (!script->images.Contains(name))
			return nullptr;
		return new HideInstruction(script, rpyLine, name, at, with);
	};
};
