#pragma once

#include "RpyInstructionHelpers.h"

struct ElseInstruction : public RpyInstruction
{
public:
  ElseInstruction(URpyScript *script, FRpyLine *rpyLine) : RpyInstruction(script, rpyLine) {};
  virtual RpyInstructionType Type() const { return RpyInstructionType::Else; }
};
