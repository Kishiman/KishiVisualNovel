
#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"

/*
stop music volume 0.25 fadeout 1.0 fadein 1.0
*/
// stop music volume 0.25 fadeout 1.0 fadein 1.0
struct StopAudioParser : public RpyParser
{
  StopAudioParser() : RpyParser(6, "^stop " + reg_name + "(?: volume " + reg_ufloatUnit + ")?(?: fadeout " + reg_ufloat + ")?(?: fadein " + reg_ufloat + ")?(?: (loop|noloop))?(?: (if_changed))?" + reg_comment, "StopAudioParser") {};

  virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
  {
    FName channel = FName(*params[0]);
    FRpyAudioOptions options;
    options.volume = GetFloat(params[1]);
    options.fadeOut = GetFloat(params[2]);
    options.fadeIn = GetFloat(params[3]);
    if (params[4] != "")
      options.loop = params[4] == "loop" ? ESchrodBool::ETrue : ESchrodBool::ETrue;
    options.if_changed = params[5] != "" ? true : false;
    return new StopInstruction(script, rpyLine, channel, options);
  };
};