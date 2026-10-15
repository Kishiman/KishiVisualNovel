#pragma once

#include "RpyInstructionHelpers.h"

struct QueueInstruction : public RpyInstruction
{
  FName channel;
  FName name;
  QueueInstruction(URpyScript *script, FRpyLine *rpyLine, FName channel, FName name, FRpyAudioOptions options) : RpyInstruction(script, rpyLine), channel(channel), name(name) { Props = PropsFromAudioOptions(options); };
  virtual bool Execute(URpySession *session)
  {
    const FRpyAudioOptions options = AudioOptionsFromProps(Props);
    auto rpyAudio = script->audios[name];
    if (!rpyAudio.audio)
      return false;
    // TODO handle state
    auto audioManager = IRpyScriptInterpreter::Execute_GetAudioManager(session->interpreter.GetObject());

    return IRpyAudioManager::Execute_QueueAudio(audioManager.GetObject(), channel, rpyAudio, options);
  };
};
