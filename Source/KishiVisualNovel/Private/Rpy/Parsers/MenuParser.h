#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"

// menu:
struct MenuParser : public RpyParser
{
  MenuParser() : RpyParser(0, "^menu:" + reg_comment, "MenuParser") {};
  virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
  {
    return new MenuInstruction(script, rpyLine);
  };
};