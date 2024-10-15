#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"

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