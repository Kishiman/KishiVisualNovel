#pragma once

#include "RpyInstructionHelpers.h"

struct StopInstruction : public RpyInstruction
{
  FName channel;
  StopInstruction(URpyScript *script, FRpyLine *rpyLine, FName channel, FRpyAudioOptions options) : RpyInstruction(script, rpyLine), channel(channel) { Props = PropsFromAudioOptions(options); };
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::STOP; }
  virtual bool Execute(URpySession *session)
  {
    const FRpyAudioOptions options = AudioOptionsFromProps(Props);
    auto existing = ArrayLib::FindIndexByPredicate(session->audioState.audioStates, [&](const FRpyAudioPlayState &state)
                                                   { return state.channel == channel; });
    if (existing != INDEX_NONE)
    {
      session->audioState.audioStates.RemoveAt(existing);
    }
    auto audioManager = IRpyScriptInterpreter::Execute_GetAudioManager(session->interpreter.GetObject());

    return IRpyAudioManager::Execute_StopAudio(audioManager.GetObject(), channel, options);
  };
};
