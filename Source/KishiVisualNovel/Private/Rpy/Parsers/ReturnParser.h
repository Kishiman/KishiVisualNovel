#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"

struct ReturnParser : public RpyParser
{
  ReturnParser() : RpyParser(0, "^return" + reg_comment, "ReturnParser") {};
  virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
  {
    return new ReturnInstruction(script, rpyLine);
  };
};