#pragma once

#include "Interfaces/C++/RpyInstruction.h"

class SayInstruction : public IRpyInstruction {
public:
  FName name;
  FString statement;
  
  SayInstruction(FName _name,FString _statement):isAsync(false),name(_name),statement(_statement){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) override;
};