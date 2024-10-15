#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"

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