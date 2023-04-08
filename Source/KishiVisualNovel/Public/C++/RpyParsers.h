// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <regex>
#include <string>

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "C++/RpyParser.h"
#include "C++/RpyInstructions.h"

/**
 */
//"init:"
struct InitParser : public RpyParser
{
	InitParser() { query = "^init:$"; };
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		RpyInstruction *init = new RpyInstruction(script, rpyLine);
		script->init = init;
		return init;
	};
};
//"$ e = Character('Eileen')"
struct DefineCharacterParser : public RpyParser
{
	DefineCharacterParser() { query = "^\\$ (\\w+) = Character\\(('\\w+')\\)$"; };
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FName varName = FName(*params[0]);
		FName characterName = FName(*params[1]);
		script->compileData.names.Add(varName, characterName);
		return new RpyInstruction(script, rpyLine);
	};
};

//"label start:"
struct LabelParser : public RpyParser
{
	LabelParser() { query = "^label (\\w+):$"; };
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		RpyInstruction *label = new LabelInstruction(script, rpyLine);
		script->labels.Add(FName(*params[0]), label);
		return label;
	};
};

//"\"Sylvie\" \"Hi there! how was class?\""
struct SayParser : public RpyParser
{
	SayParser() { query = "^(?:(\\w+|\".+\") )?\"(.*)\"$"; };
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		bool literal = params[0].StartsWith("\"") && params[0].EndsWith("\"");
		if (literal)
			params[0] = params[0].Mid(1, params[0].Len() - 2);
		FName name = FName(*params[0]);
		if (!literal)
		{
			if (script->compileData.names.Contains(name))
			{
				name = script->compileData.names[name];
			}
			else
			{
				return nullptr;
			}
		}
		return new SayInstruction(script, rpyLine, name, params[1]);
	};
};
//"\"Hope everything is okay\""
struct SayParser2 : public RpyParser
{
	SayParser2() { query = "^\"(.*)\"$"; };
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FName name = FName("");
		return new SayInstruction(script, rpyLine, name, params[0]);
	};
};

struct ImageParser : public RpyParser
{
	ImageParser() { query = "^image((\\s\\w+)+) = \"([/\\w\\.\\s]+)\"$"; };
	virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
	{
		FName name = FName(*params[0]);
		FString path = params[2];
		UPaperSprite *image = Cast<UPaperSprite>(StaticLoadObject(UPaperSprite::StaticClass(), NULL, *path));
		if (image)
		{
			script->images.Add(name, image);
			return new RpyInstruction(script, rpyLine);
		}

		return nullptr;
	};
};
struct ShowParser : public RpyParser
{
	ShowParser() { query = "^show(( (?!at|with)\\w+)+)(?: at (\\w+))?(?: with (\\w+))?$"; };
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
// if true :
struct IfBoolParser : public RpyParser
{
	IfBoolParser() { query = "^if (!)?(\\w+|True|False):$"; };
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