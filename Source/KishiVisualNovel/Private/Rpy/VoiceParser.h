
#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"

// voice "waves.opus"
struct VoiceParser : public RpyParser
{
  VoiceParser() : RpyParser(1, "^voice " + reg_path + reg_comment, "VoiceParser") {};
  virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
  {
    FName channel = FName("voice");
    FString path = GetPath(params[0]);
    FName saveName = FName(*path);
    FRpyAudioOptions options;
    if (!script->audios.Contains(saveName))
    {
      if (!script->AddDefaultAudio(saveName, path))
        return nullptr;
    }
    return new PlayInstruction(script, rpyLine, channel, saveName, options);
  };
};