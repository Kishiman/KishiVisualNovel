#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"

// else:
struct ElseParser : public RpyParser
{
  ElseParser() : RpyParser(0, "^else\\s*:" + reg_comment, "ElseParser") {};
  virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
  {
    return new ElseInstruction(script, rpyLine);
  };
};