
#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"

// play music "waves.opus" volume 0.25 fadeout 1.0 fadein 1.0
// play music track_1 volume 0.25 fadeout 1.0 fadein 1.0
struct AudioParser : public RpyParser
{
  AudioParser() : RpyParser(9, "^(play|queue) " + reg_name + " (?:" + reg_name + "|" + reg_path + ")(?: volume " + reg_ufloatUnit + ")?(?: fadeout " + reg_ufloat + ")?(?: fadein " + reg_ufloat + ")?(?: (loop|noloop))?(?: (if_changed))?" + reg_comment, "AudioParser") {};
  virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
  {
    auto cmd = params[0];
    FName channel = FName(*params[1]);
    FString path;
    if (params[2] != "")
    {
      path = params[2];
    }
    else if (params[3] != "")
    {
      path = GetPath(params[3]);
    }
    FName name = FName(*path);
    FRpyAudioOptions options;
    options.volume = GetFloat(params[4]);
    options.fadeOut = GetFloat(params[5]);
    options.fadeIn = GetFloat(params[6]);
    if (params[7] != "")
      options.loop = params[7] == "loop" ? ESchrodBool::ETrue : ESchrodBool::ETrue;
    else if (channel == FName("music"))
      options.loop = ESchrodBool::ETrue;
    options.if_changed = params[8] != "" ? true : false;
    if (!script->audios.Contains(name))
    {
      if (!script->AddDefaultAudio(name, path))
        return nullptr;
    }
    if (cmd == "queue")
      return new QueueInstruction(script, rpyLine, channel, name, options);
    return new PlayInstruction(script, rpyLine, channel, name, options);
  };
};