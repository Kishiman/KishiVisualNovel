#pragma once

#include "RpyInstructionHelpers.h"

struct ChoiceInstruction : public RpyInstruction
{
  FString statement;
  ChoiceInstruction(URpyScript *script, FRpyLine *rpyLine, FString statement) : RpyInstruction(script, rpyLine), statement(statement) {};
  virtual RpyInstructionType Type() const
  {
    return RpyInstructionType::Choice;
  }
  virtual bool Compile() override;
};
