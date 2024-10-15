#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"

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