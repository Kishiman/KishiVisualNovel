#pragma once

#include "RpyInstructionHelpers.h"
#include "IfInstruction.h"

class IfBoolInstruction : public IfInstruction
{
public:
  FName varName;

  IfBoolInstruction(URpyScript *script, FRpyLine *rpyLine, FName varName, bool reverse) : IfInstruction(script, rpyLine), varName(varName) { Props.SetBool(FName(TEXT("reverse")), reverse); };

  virtual bool Compile() override
  {
    if (varName != FName(TEXT("True")) && varName != FName(TEXT("False")) && !script->compileData.bools.Contains(varName))
      script->compileData.setValue(varName, false);

    return IfInstruction::Compile();
  }

  virtual bool Execute(URpySession *session)
  {
    if (varName == FName(TEXT("True")))
      this->condition = true;
    else if (varName == FName(TEXT("False")))
      this->condition = false;
    else if (!session->runtimeData.bools.Contains(this->varName))
    {
      UE_LOG(LogTemp, Fatal, TEXT("bool variable name %s does not exist in Runtime data"), *this->varName.ToString());
      return false;
    }
    else
      this->condition = session->runtimeData.bools[this->varName];
    bool bReverse = false;
    Props.GetBool(FName(TEXT("reverse")), bReverse);
    this->condition = this->condition ^ bReverse;
    return true;
  };
};
