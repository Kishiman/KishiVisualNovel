
#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"
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