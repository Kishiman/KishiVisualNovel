#pragma once

#include "RpyInstructionHelpers.h"

struct JumpInstruction : public RpyInstruction
{
  FName label;

  JumpInstruction(URpyScript *script, FRpyLine *rpyLine, FName label) : RpyInstruction(script, rpyLine), label(label) {};
  virtual bool Compile()
  {
    return this->script->labels.Contains(this->label);
  };
  virtual RpyInstruction *GetNext(URpySession *session)
  {
    return this->script->labels[this->label];
  };
};
