#pragma once

#include "RpyInstructionHelpers.h"

struct ReturnInstruction : public RpyInstruction
{
  ReturnInstruction(URpyScript *script, FRpyLine *rpyLine) : RpyInstruction(script, rpyLine) {};
  virtual RpyInstruction *GetNext(URpySession *session)
  {
    if (session->callStack.Num() == 0)
      return nullptr;
    return session->callStack.Pop();
  };
};
