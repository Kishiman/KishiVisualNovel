#pragma once

#include "Interfaces/C++/RpyInstruction.h"


class BlankInstruction : public URpyInstructionBase {
public:
  BlankInstruction(){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) {return true;};
};

class SceneInstruction : public URpyInstructionBase {
public:
  TMap<FName, FString> params;
  
  SceneInstruction(TMap<FName, FString> _params):params(_params){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) override;
};

class ShowInstruction : public URpyInstructionBase {
public:
  FName name;
  TMap<FName, FString> params;
  
  ShowInstruction(FName _name,TMap<FName, FString> _params):name(_name),params(_params){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) override;
};

class HideInstruction : public URpyInstructionBase {
public:
  FName name;
  TMap<FName, FString> params;
  
  HideInstruction(FName _name,TMap<FName, FString> _params):name(_name),params(_params){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) override;
};

class SayInstruction : public URpyInstructionBase {
public:
  FName name;
  FString statement;
  
  SayInstruction(FName _name,FString _statement):name(_name),statement(_statement){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) override;
};
class PlayInstruction : public URpyInstructionBase {
public:
  FName name;
  TMap<FName, FString> params;

  PlayInstruction(FName _name,TMap<FName, FString> _params):name(_name),params(_params){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) override;
};

class PauseInstruction : public URpyInstructionBase {
public:
  float timeout;

  PauseInstruction(float timeout):timeout(timeout){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) override;
};