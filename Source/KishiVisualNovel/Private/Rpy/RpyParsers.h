// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <regex>

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"
#include <string>

#include "Utils/MapUtils.h"

//"init:"
struct InitParser : public RpyParser
{
	InitParser() : RpyParser(0, "^init:" + reg_comment, "InitParser") {};
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
	DefineStringParser() : RpyParser(3, "^(\\$|default) " + reg_var_name + " = " + reg_string + reg_comment, "DefineStringParser") {};
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
//$ variable_name = True
struct DefineBoolParser : public RpyParser
{
	DefineBoolParser() : RpyParser(3, "^(\\$|default) " + reg_var_name + " = " + reg_bool + reg_comment, "DefineBoolParser") {};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		bool isDefault = params[0] == "default";
		FName varName = FName(*params[1]);
		bool value = GetBool(params[2]);
		if (isDefault)
			script->compileData.bools.Add(varName, value);
		return new AssignInstruction<bool>(script, rpyLine, varName, value);
	};
};
//"$ e = Character('Eileen')"
struct DefineCharacterParser : public RpyParser
{
	DefineCharacterParser() : RpyParser(3, "^(?:define|\\$) " + reg_var_name + " = Character\\(" + reg_string_simple + reg_args_map + "?\\)" + reg_comment, "DefineCharacterParser") {};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FName varName = FName(*params[0]);
		params[1] = GetString(params[1]);
		auto args = GetArgs(params[2]);
		FRpyCharacter character;
		character.name = FName(*params[1]);
		character.image = FName(MapUtils::FindOrMake(args, FName("image")));
		character.voice = FName(MapUtils::FindOrMake(args, FName("voice_tag")));
		// TO FIX
		character.color = FColor::FromHex(MapUtils::FindOrMake(args, FName("color")));
		script->characters.Add(varName, character);
		return new RpyInstruction(script, rpyLine);
	};
};
/*
define audio.sunflower = "music/sun-flower-slow-jam"
define image = "images/ok"
define music = "music/ok"
*/
struct DefineMediaParser : public RpyParser
{
	DefineMediaParser() : RpyParser(3, "^(?:define )?(audio|image|music|sound|voice) " + reg_multi_name + " = " + reg_path + reg_comment, "DefineMediaParser") {};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FString mediaPath = params[0];
		FName media = FName(*mediaPath);
		FName name = FName(*params[1]);
		FString path = GetPath(params[2]);
		auto names = RpyParser::GetNames(path);
		if (media == FName("audio") || media == FName("music") || media == FName("sound") || media == FName("voice"))
		{
			if (!script->audios.Contains(name))
				if (!script->AddDefaultAudio(name, path))
					return nullptr;
			return new RpyInstruction(script, rpyLine);
		}
		if (media == FName("image"))
		{
			if (!script->images.Contains(name))
				if (!script->AddDefaultImage(name, path))
					return nullptr;
			return new RpyInstruction(script, rpyLine);
		}
		return nullptr;
	};
};
/*
stop music volume 0.25 fadeout 1.0 fadein 1.0
*/
// stop music volume 0.25 fadeout 1.0 fadein 1.0
struct StopAudioParser : public RpyParser
{
	StopAudioParser() : RpyParser(6, "^stop " + reg_name + "(?: volume " + reg_ufloatUnit + ")?(?: fadeout " + reg_ufloat + ")?(?: fadein " + reg_ufloat + ")?(?: (loop|noloop))?(?: (if_changed))?" + reg_comment, "StopAudioParser") {};

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
	VoiceParser() : RpyParser(1, "^voice " + reg_path + reg_comment, "VoiceParser") {};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FName channel = FName("voice");
		FString path = GetPath(params[0]);
		FName saveName = FName(*path);
		FRpyAudioOptions options;
		if (!script->audios.Contains(saveName))
		{
			if (!script->AddDefaultAudio(saveName, path))
				return nullptr;
		}
		return new PlayInstruction(script, rpyLine, channel, saveName, options);
	};
};

// play music "waves.opus" volume 0.25 fadeout 1.0 fadein 1.0
// play music track_1 volume 0.25 fadeout 1.0 fadein 1.0
struct AudioParser : public RpyParser
{
	AudioParser() : RpyParser(9, "^(play|queue) " + reg_name + " (?:" + reg_name + "|" + reg_path + ")(?: volume " + reg_ufloatUnit + ")?(?: fadeout " + reg_ufloat + ")?(?: fadein " + reg_ufloat + ")?(?: (loop|noloop))?(?: (if_changed))?" + reg_comment, "AudioParser") {};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		auto cmd = params[0];
		FName channel = FName(*params[1]);
		FString path;
		if (params[2] != "")
		{
			path = params[2];
		}
		else if (params[3] != "")
		{
			path = GetPath(params[3]);
		}
		FName name = FName(*path);
		FRpyAudioOptions options;
		options.volume = GetFloat(params[4]);
		options.fadeOut = GetFloat(params[5]);
		options.fadeIn = GetFloat(params[6]);
		if (params[7] != "")
			options.loop = params[7] == "loop" ? ESchrodBool::ETrue : ESchrodBool::ETrue;
		else if (channel == FName("music"))
			options.loop = ESchrodBool::ETrue;
		options.if_changed = params[8] != "" ? true : false;
		if (!script->audios.Contains(name))
		{
			if (!script->AddDefaultAudio(name, path))
				return nullptr;
		}
		if (cmd == "queue")
			return new QueueInstruction(script, rpyLine, channel, name, options);
		return new PlayInstruction(script, rpyLine, channel, name, options);
	};
};

// label start:
struct LabelParser : public RpyParser
{
	LabelParser() : RpyParser(1, "^label " + reg_name + ":" + reg_comment, "LabelParser") {};
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
	JumpParser() : RpyParser(1, "^jump (\\w+)" + reg_comment, "JumpParser") {};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		return new JumpInstruction(script, rpyLine, FName(*params[0]));
	};
};

// pause 2.0
struct PauseParser : public RpyParser
{
	PauseParser() : RpyParser(1, "^pause " + reg_ufloat + reg_comment, "PauseParser") {};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		return new PauseInstruction(script, rpyLine, RpyParser::GetFloat(params[0]));
	};
};
struct ReturnParser : public RpyParser
{
	ReturnParser() : RpyParser(0, "^return" + reg_comment, "ReturnParser") {};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		return new ReturnInstruction(script, rpyLine);
	};
};

//"\"Sylvie\" \"Hi there! how was class?\""
struct SayParser : public RpyParser
{
	SayParser() : RpyParser(3, "^(?:" + reg_string_simple + " )?" + reg_string + "(?: with (\\w+))?" + reg_comment, "SayParser") {};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FName name = FName(*GetString(params[0]));
		FString statement = GetString(params[1]);
		FName with = FName(*params[2]);
		return new SayInstruction(script, rpyLine, name, statement, with);
	};
};

struct SceneParser : public RpyParser
{
	SceneParser() : RpyParser(2, "^scene " + reg_multi_name + reg_rpy_options + reg_comment, "SceneParser") {};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FName name = FName(*params[0]);
		auto names = RpyParser::GetNames(name.ToString());
		if (!script->images.Contains(name))
			if (!script->AddDefaultImage(name, params[0]))
				return nullptr;
		FRpyImageOptions options;
		auto rpyOptions = GetRpyOptions(params[1]);
		options.with = FName(MapUtils::FindOrMake(rpyOptions, FName("with")));
		return new SceneInstruction(script, rpyLine, name, options);
	};
};

// ^show
// show john happy
// show sylvie green smile with dissolve
struct ShowParser : public RpyParser
{
	ShowParser() : RpyParser(2, "^show " + reg_multi_name + reg_rpy_options + reg_comment, "ShowParser") {};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FName name = FName(*params[0]);
		auto names = RpyParser::GetNames(name.ToString());
		auto mainName = names[0];
		auto rpyOptions = GetRpyOptions(params[1]);
		FName at = FName(MapUtils::FindOrMake(rpyOptions, FName("at")));
		FName with = FName(MapUtils::FindOrMake(rpyOptions, FName("with")));
		auto image = script->images.Find(name);
		auto layeredImage = script->layeredImages.Find(mainName);
		if (!(image || layeredImage))
		{
			if (!script->AddDefaultImage(name, params[0]))
				return nullptr;
		}
		image = script->images.Find(name);
		if (image)
			return new ShowInstruction(script, rpyLine, name, at, with);
		layeredImage = script->layeredImages.Find(mainName);
		if (layeredImage)
		{
			names.RemoveAt(0);
			return new ShowLayeredInstruction(script, rpyLine, mainName, names, at, with);
		}
		return nullptr;
	};
};

// if True:
struct IfBoolParser : public RpyParser
{
	IfBoolParser() : RpyParser(2, "^if (!)?(\\w+|True|False):" + reg_comment, "IfBoolParser") {};
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
// else:
struct ElseParser : public RpyParser
{
	ElseParser() : RpyParser(0, "^else\\s*:" + reg_comment, "ElseParser") {};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		return new ElseInstruction(script, rpyLine);
	};
};
// menu:
struct MenuParser : public RpyParser
{
	MenuParser() : RpyParser(0, "^menu:" + reg_comment, "MenuParser") {};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		return new MenuInstruction(script, rpyLine);
	};
};
// "Yes":
struct ChoiceParser : public RpyParser
{
	ChoiceParser() : RpyParser(1, "^" + RpyParser::reg_string + ":" + reg_comment, "ChoiceParser") {};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		return new ChoiceInstruction(script, rpyLine, RpyParser::GetString(params[0]));
	};
};

//"call start"
struct CallParser : public RpyParser
{
	CallParser() : RpyParser(1, "^call (\\w+)" + reg_comment, "CallParser") {};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FName name = FName(*params[0]);
		auto call = new CallInstruction(script, rpyLine, name);
		return call;
	};
};

struct CharacterSayParser : public RpyParser
{
	CharacterSayParser() : RpyParser(5, "^" + reg_var_name + "(?: " + reg_multi_name + ")?(?: @ " + reg_multi_name + ")? " + reg_string + reg_rpy_options + reg_comment, "CharacterSayParser") {};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FName name = FName(*params[0]);
		FName image = FName(*params[1]);
		FName atImage = FName(*params[2]);
		FString statement = GetString(params[3]);
		auto rpyOptions = GetRpyOptions(params[4]);
		FName with = FName(MapUtils::FindOrMake(rpyOptions, FName("with")));
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

// hide sylvie
struct HideParser : public RpyParser
{
	HideParser() : RpyParser(2, "^hide " + reg_multi_name + reg_rpy_options + reg_comment, "HideParser") {};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{

		FName name = RpyParser::GetNames(*params[0])[0];
		auto rpyOptions = GetRpyOptions(params[1]);
		FName at = FName(MapUtils::FindOrMake(rpyOptions, FName("at")));
		FName with = FName(MapUtils::FindOrMake(rpyOptions, FName("with")));
		for (auto &pair : script->images)
		{
			if (pair.Value.tag == name)
				return new HideInstruction(script, rpyLine, name, at, with);
		}
		return nullptr;
	};
};

InitParser _InitParser;
DefineCharacterParser _DefineCharacterParser;
DefineMediaParser _DefineMediaParser;
DefineStringParser _DefineStringParser;
DefineBoolParser _DefineBoolParser;
StopAudioParser _StopAudioParser;
VoiceParser _VoiceParser;
AudioParser _AudioParser;
LabelParser _LabelParser;
JumpParser _JumpParser;
PauseParser _PauseParser;
ReturnParser _ReturnParser;
CallParser _CallParser;
MenuParser _MenuParser;
ChoiceParser _ChoiceParser;
SayParser _SayParser;
IfBoolParser _IfBoolParser;
ElseParser _ElseParser;
CharacterSayParser _CharacterSayParser;
ShowParser _ShowParser;
SceneParser _SceneParser;
HideParser _HideParser;
TArray<RpyParser *> RpyParser::parsers = {
		&_InitParser,
		&_DefineCharacterParser,
		&_DefineMediaParser,
		&_DefineStringParser,
		&_DefineBoolParser,
		&_StopAudioParser,
		&_VoiceParser,
		&_AudioParser,
		&_LabelParser,
		&_JumpParser,
		&_PauseParser,
		&_ReturnParser,
		&_CallParser,
		&_MenuParser,
		&_ChoiceParser,
		&_SayParser,
		&_IfBoolParser,
		&_ElseParser,
		&_CharacterSayParser,
		&_ShowParser,
		&_SceneParser,
		&_HideParser};