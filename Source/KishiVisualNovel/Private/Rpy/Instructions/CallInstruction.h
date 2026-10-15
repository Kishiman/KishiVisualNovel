#pragma once

#include "RpyInstructionHelpers.h"
#include "JumpInstruction.h"

struct CallInstruction : public JumpInstruction
{
  CallInstruction(URpyScript *script, FRpyLine *rpyLine, FName name) : JumpInstruction(script, rpyLine, name) {};
  virtual RpyInstruction *GetNext(URpySession *session)
  {
    session->callStack.Add(next);
    return JumpInstruction::GetNext(session);
  };
};
