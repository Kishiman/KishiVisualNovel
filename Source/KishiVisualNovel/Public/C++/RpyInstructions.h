#pragma once

#include "C++/RpyInstruction.h"

class BlankInstruction : public RpyInstruction
{
public:
  BlankInstruction(URpyScript *script, FRpyLine *rpyLine) : RpyInstruction(script, rpyLine){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter) { return true; };
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
  TMap<FName, FString> params;

  ShowInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, TMap<FName, FString> params) : RpyInstruction(script, rpyLine), name(name), params(params){};
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