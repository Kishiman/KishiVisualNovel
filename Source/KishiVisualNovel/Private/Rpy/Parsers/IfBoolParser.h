#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"

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