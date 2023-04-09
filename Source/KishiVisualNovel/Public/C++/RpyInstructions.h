#pragma once

#include "C++/RpyInstruction.h"
#include "Interfaces/RpyInterpreter.h"

class LabelInstruction : public RpyInstruction
{
public:
  LabelInstruction(URpyScript *script, FRpyLine *rpyLine) : RpyInstruction(script, rpyLine){};
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

class SceneInstruction : public RpyInstruction
{
public:
  TMap<FName, FString> params;

  SceneInstruction(URpyScript *script, FRpyLine *rpyLine, TMap<FName, FString> params) : RpyInstruction(script, rpyLine), params(params){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter)
  {
    RpyInstruction::Execute(interpreter);
    return IRpyInterpreter::Execute_Scene(interpreter.GetObject(), this->params);
  };
};

class ShowInstruction : public RpyInstruction
{
public:
  FName name;
  FName at;
  FName with;

  ShowInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, FName at, FName with) : RpyInstruction(script, rpyLine), name(name), at(at), with(with){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter)
  {
    RpyInstruction::Execute(interpreter);
    UPaperSprite *image = script->images[name];
    if (!image)
      return false;
    return IRpyInterpreter::Execute_Show(interpreter.GetObject(), image, at, with);
  };
};

class HideInstruction : public RpyInstruction
{
public:
  FName name;
  TMap<FName, FString> params;

  HideInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, TMap<FName, FString> params) : RpyInstruction(script, rpyLine), name(name), params(params){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter)
  {
    RpyInstruction::Execute(interpreter);
    return IRpyInterpreter::Execute_Hide(interpreter.GetObject(), this->name, this->params);
  };
};

class SayInstruction : public RpyInstruction
{
public:
  FName name;
  FString statement;

  SayInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, FString statement) : RpyInstruction(script, rpyLine), name(name), statement(statement){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter)
  {
    RpyInstruction::Execute(interpreter);
    return IRpyInterpreter::Execute_Say(interpreter.GetObject(), this->name, this->statement);
  };
};
class PlayInstruction : public RpyInstruction
{
public:
  FName name;
  TMap<FName, FString> params;

  PlayInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, TMap<FName, FString> params) : RpyInstruction(script, rpyLine), name(name), params(params){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter)
  {
    RpyInstruction::Execute(interpreter);
    return IRpyInterpreter::Execute_PlayMusic(interpreter.GetObject(), this->name, this->params);
  };
};

class PauseInstruction : public RpyInstruction
{
public:
  float timeout;

  PauseInstruction(URpyScript *script, FRpyLine *rpyLine, float timeout) : RpyInstruction(script, rpyLine), timeout(timeout){};
  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter)
  {
    RpyInstruction::Execute(interpreter);
    return IRpyInterpreter::Execute_Pause(interpreter.GetObject(), this->timeout);
  };
};

class ElseInstruction;
//^\s*if\s+[\w\s\.<>=!]+:\s*$
class IfInstruction : public RpyInstruction
{
protected:
  bool condition;
  ElseInstruction *elseInstruction = nullptr;

public:
  IfInstruction(URpyScript *script, FRpyLine *rpyLine, bool condition = true) : RpyInstruction(script, rpyLine), condition(condition){};
  virtual RpyInstructionType Type() const
  {
    return RpyInstructionType::If;
  }
  virtual bool Compile() override;
  virtual RpyInstruction *GetNext(const TScriptInterface<IRpyInterpreter> &interpreter) override;
};
class ElseInstruction : public RpyInstruction
{
public:
  bool condition = false;

  ElseInstruction(URpyScript *script, FRpyLine *rpyLine) : RpyInstruction(script, rpyLine){};
  virtual RpyInstructionType Type() const
  {
    return RpyInstructionType::Else;
  }
  virtual RpyInstruction *GetNext(const TScriptInterface<IRpyInterpreter> &interpreter)
  {
    if (this->condition)
    {
      this->condition = false;
      return this->children[0];
    }
    return this->next;
  };
};

class IfBoolInstruction : public IfInstruction
{
public:
  FName varName;
  bool reverse;

  IfBoolInstruction(URpyScript *script, FRpyLine *rpyLine, FName varName, bool reverse) : IfInstruction(script, rpyLine), varName(varName), reverse(reverse){};

  virtual bool Execute(const TScriptInterface<IRpyInterpreter> &interpreter)
  {
    this->condition = IRpyInterpreter::Execute_GetBool(interpreter.GetObject(), this->varName);
    this->condition = this->condition ^ this->reverse;
    return true;
  };
};
class JumpInstruction : public RpyInstruction
{
  FName label;

public:
  JumpInstruction(URpyScript *script, FRpyLine *rpyLine, FName label) : RpyInstruction(script, rpyLine), label(label){};
  virtual bool Compile()
  {
    return this->script->labels.Contains(this->label);
  };
  virtual RpyInstruction *GetNext(const TScriptInterface<IRpyInterpreter> &interpreter)
  {
    return this->script->labels[this->label];
  };
};

bool IfInstruction::Compile()
{
  RpyInstruction *current = this;
  while (current->next)
  {
    // check if the instruction is of type else and in the same tabs scope
    if (!!(current->next->Type() & RpyInstructionType::Else) && current->next->rpyLine->tabs == this->rpyLine->tabs)
    {
      current = current->next;
    }
    else
    {
      break;
    }
  }
  this->next = current->next;
  if (current != this)
  {
    this->elseInstruction = (ElseInstruction *)(current);
  }
  if (children.Num() > 0)
    children.Last()->next = this->next;
  return true;
}
RpyInstruction *IfInstruction::GetNext(const TScriptInterface<IRpyInterpreter> &interpreter)
{
  if (this->condition)
  {
    return this->children[0];
  }
  else if (this->elseInstruction)
  {
    this->elseInstruction->condition = true;
    return this->elseInstruction;
  }
  return this->next;
};
