// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <regex>

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"
#include <string>

#include "Utils/MapUtils.h"

#include "./AudioParser.h"
#include "./DefineParser.h"
#include "./InitParser.h"
#include "./LabelParser.h"
#include "./JumpParser.h"
#include "./SceneParser.h"
#include "./ShowParser.h"
#include "./StopAudioParser.h"
#include "./VoiceParser.h"

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
	HideParser() : RpyParser(7, "^hide " + reg_multi_name + reg_rpy_scene_options + reg_comment, "HideParser") {};
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{

		FName tag = RpyParser::GetNames(*params[0])[0];
		auto options = GetRpySceneOptions(params, 1);
		for (auto &pair : script->images)
		{
			if (pair.Value.tag == tag)
				return new HideInstruction(script, rpyLine, tag, options);
		}
		for (auto &pair : script->layeredImages)
		{
			if (pair.Value.tag == tag)
				return new HideInstruction(script, rpyLine, tag, options);
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