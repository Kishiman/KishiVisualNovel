
#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"
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