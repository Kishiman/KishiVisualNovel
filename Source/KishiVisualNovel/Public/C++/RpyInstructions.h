#pragma once

#include "C++/RpyInstruction.h"

class LabelInstruction : public RpyInstruction
{
public:
  LabelInstruction(URpyScript *script, FRpyLine *rpyLine) : RpyInstruction(script, rpyLine){};
  virtual bool Compile() override;
};

class SceneInstruction : public RpyInstruction
{
public:
  TMap<FName, FString> params;

  SceneInstruction(URpyScript *script, FRpyLine *rpyLine, TMap<FName, FString> params) : RpyInstruction(script, rpyLine), params(params){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) override;
};

class ShowInstruction : public RpyInstruction
{
public:
  FName name;
  FName at;
  FName with;

  ShowInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, FName at, FName with) : RpyInstruction(script, rpyLine), name(name), at(at), with(with){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) override;
};

class HideInstruction : public RpyInstruction
{
public:
  FName name;
  TMap<FName, FString> params;

  HideInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, TMap<FName, FString> params) : RpyInstruction(script, rpyLine), name(name), params(params){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) override;
};

class SayInstruction : public RpyInstruction
{
public:
  FName name;
  FString statement;

  SayInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, FString statement) : RpyInstruction(script, rpyLine), name(name), statement(statement){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) override;
};
class PlayInstruction : public RpyInstruction
{
public:
  FName name;
  TMap<FName, FString> params;

  PlayInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, TMap<FName, FString> params) : RpyInstruction(script, rpyLine), name(name), params(params){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) override;
};

class PauseInstruction : public RpyInstruction
{
public:
  float timeout;

  PauseInstruction(URpyScript *script, FRpyLine *rpyLine, float timeout) : RpyInstruction(script, rpyLine), timeout(timeout){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) override;
};
//^\s*if\s+[\w\s\.<>=!]+:\s*$
class IfInstruction : public RpyInstruction
{
public:
  bool condition = true;

  IfInstruction(URpyScript *script, FRpyLine *rpyLine, bool condition = true) : RpyInstruction(script, rpyLine), condition(condition){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) override { return true; };
  virtual RpyInstruction *GetNext(const TScriptInterface<IRpyInterpreter> &interpreter) override;
};

class IfBoolInstruction : public IfInstruction
{
public:
  FName varName;
  bool reverse;

  IfBoolInstruction(URpyScript *script, FRpyLine *rpyLine, FName varName, bool reverse) : IfInstruction(script, rpyLine), varName(varName), reverse(reverse){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) override;
};
class JumpInstruction : public RpyInstruction
{
  FName label;

public:
  JumpInstruction(URpyScript *script, FRpyLine *rpyLine, FName label) : RpyInstruction(script, rpyLine), label(label){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) override;
};