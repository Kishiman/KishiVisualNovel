#pragma once

#include "RpyInstructionHelpers.h"

struct PlayInstruction : public RpyInstruction
{
  FName channel;
  FName name;

  PlayInstruction(URpyScript *script, FRpyLine *rpyLine, FName channel, FName name, FRpyAudioOptions options) : RpyInstruction(script, rpyLine), channel(channel), name(name) { Props = PropsFromAudioOptions(options); };
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::PLAY; }
  virtual bool Execute(URpySession *session)
  {
    const FRpyAudioOptions options = AudioOptionsFromProps(Props);
    auto rpyAudio = script->audios[name];
    if (!rpyAudio.audio)
      return false;

    auto existing = session->audioState.audioStates.FindByPredicate([&](const FRpyAudioPlayState &state)
                                                                    { return state.channel == channel; });
    if (existing)
    {
      existing->rpyAudio = rpyAudio;
      existing->options = options;
    }
    else
    {
      session->audioState.audioStates.Add(FRpyAudioPlayState{channel, rpyAudio, options});
    }
    auto audioManager = IRpyScriptInterpreter::Execute_GetAudioManager(session->interpreter.GetObject());

    return IRpyAudioManager::Execute_PlayAudio(audioManager.GetObject(), channel, rpyAudio, options);
  };
};
