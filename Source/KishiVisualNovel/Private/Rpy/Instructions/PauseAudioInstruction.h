#pragma once

#include "RpyInstructionHelpers.h"

struct PauseAudioInstruction : public RpyInstruction
{
  float timeout;

  PauseAudioInstruction(URpyScript *script, FRpyLine *rpyLine, float timeout) : RpyInstruction(script, rpyLine), timeout(timeout) {};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::PAUSE; }
  virtual bool Execute(URpySession *session)
  {
    session->audioState.audioStates.Empty();
    auto audioManager = IRpyScriptInterpreter::Execute_GetAudioManager(session->interpreter.GetObject());

    return IRpyAudioManager::Execute_PauseAudio(audioManager.GetObject(), this->timeout);
  };
};

//^\s*if\s+[\w\s\.<>=!]+:\s*$
