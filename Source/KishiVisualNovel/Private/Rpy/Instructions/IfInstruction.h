#pragma once

#include "RpyInstructionHelpers.h"

struct IfInstruction : public RpyInstruction
{
protected:
  bool condition;
  ElseInstruction *elseInstruction = nullptr;

public:
  IfInstruction(URpyScript *script, FRpyLine *rpyLine, bool condition = true) : RpyInstruction(script, rpyLine), condition(condition) {};
  virtual RpyInstructionType Type() const
  {
    return RpyInstructionType::If;
  }
  virtual bool Compile() override;
  virtual RpyInstruction *GetNext(URpySession *session) override;
};
