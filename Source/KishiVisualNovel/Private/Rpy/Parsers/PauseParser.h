
#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"

// pause 2.0
struct PauseParser : public RpyParser
{
  PauseParser() : RpyParser(1, "^pause " + reg_ufloat + reg_comment, "PauseParser") {};
  virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
  {
    return new PauseInstruction(script, rpyLine, RpyParser::GetFloat(params[0]));
  };
};