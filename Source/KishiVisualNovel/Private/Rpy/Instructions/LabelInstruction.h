#pragma once

#include "RpyInstructionHelpers.h"

struct LabelInstruction : public RpyInstruction
{
  LabelInstruction(URpyScript *script, FRpyLine *rpyLine) : RpyInstruction(script, rpyLine) {};
  virtual bool Compile()
  {
    this->next = nullptr;
    auto num = this->children.Num();
    if (num > 0)
    {
      this->children[num - 1]->next = nullptr;
    }
    return true;
  };
};
