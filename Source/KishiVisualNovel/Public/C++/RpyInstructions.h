#pragma once

#include "C++/RpyInstruction.h"
#include "Interfaces/RpyInterpreter.h"
#include "RpySession.h"

// Labels & Control Flow
struct LabelInstruction : public RpyInstruction
{
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
template <typename T>
struct AssignInstruction : public RpyInstruction
{
  FName varName;
  T value;

  AssignInstruction(URpyScript *script, FRpyLine *rpyLine, FName varName, T value) : RpyInstruction(script, rpyLine), varName(varName), value(value){};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::SCENE; }
  void SetValue(URpySession *session);
  virtual bool Execute(URpySession *session, bool &autoExecuteNext)
  {
    RpyInstruction::Execute(session, autoExecuteNext);
    SetValue(session);
    return true;
  };
};
void AssignInstruction<bool>::SetValue(URpySession *session) { session->RuntimeData.bools.Add(varName, value); }
void AssignInstruction<FName>::SetValue(URpySession *session) { session->RuntimeData.names.Add(varName, value); }
void AssignInstruction<FString>::SetValue(URpySession *session) { session->RuntimeData.strings.Add(varName, value); }
void AssignInstruction<int>::SetValue(URpySession *session) { session->RuntimeData.ints.Add(varName, value); }
void AssignInstruction<float>::SetValue(URpySession *session) { session->RuntimeData.floats.Add(varName, value); }

struct SceneInstruction : public RpyInstruction
{
  TMap<FName, FString> params;

  SceneInstruction(URpyScript *script, FRpyLine *rpyLine, TMap<FName, FString> params) : RpyInstruction(script, rpyLine), params(params){};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::SCENE; }
  virtual bool Execute(URpySession *session, bool &autoExecuteNext)
  {
    RpyInstruction::Execute(session, autoExecuteNext);
    return IRpyInterpreter::Execute_Scene(session->interpreter.GetObject(), autoExecuteNext, this->params);
  };
};

struct ShowInstruction : public RpyInstruction
{
  FName name;
  FName at;
  FName with;

  ShowInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, FName at, FName with) : RpyInstruction(script, rpyLine), name(name), at(at), with(with){};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::SHOW; }
  virtual bool Execute(URpySession *session, bool &autoExecuteNext)
  {
    RpyInstruction::Execute(session, autoExecuteNext);
    auto rpyImage = script->images[name];
    if (!rpyImage.image)
      return false;
    return IRpyInterpreter::Execute_Show(session->interpreter.GetObject(), autoExecuteNext, rpyImage, at, with);
  };
};

struct HideInstruction : public RpyInstruction
{
  FName name;
  FName at;
  FName with;

  HideInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, FName at, FName with) : RpyInstruction(script, rpyLine), name(name), at(at), with(with){};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::HIDE; }
  virtual bool Execute(URpySession *session, bool &autoExecuteNext)
  {
    RpyInstruction::Execute(session, autoExecuteNext);
    auto rpyImage = script->images[name];
    if (!rpyImage.image)
      return false;
    return IRpyInterpreter::Execute_Hide(session->interpreter.GetObject(), autoExecuteNext, rpyImage, at, with);
  };
};

struct SayInstruction : public RpyInstruction
{
  FName name;
  FString statement;
  FName with;

  SayInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, FString statement, FName with = "") : RpyInstruction(script, rpyLine), name(name), statement(statement), with(with){};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::SAY; }
  virtual bool Execute(URpySession *session, bool &autoExecuteNext)
  {
    RpyInstruction::Execute(session, autoExecuteNext);
    return IRpyInterpreter::Execute_Say(session->interpreter.GetObject(), autoExecuteNext, this->name, this->statement);
  };
};
struct PlayInstruction : public RpyInstruction
{
  FName channel;
  FName name;
  FRpyAudioOptions options;

  PlayInstruction(URpyScript *script, FRpyLine *rpyLine, FName channel, FName name, FRpyAudioOptions options) : RpyInstruction(script, rpyLine), channel(channel), name(name), options(options){};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::PLAY; }
  virtual bool Execute(URpySession *session, bool &autoExecuteNext)
  {
    RpyInstruction::Execute(session, autoExecuteNext);
    auto rpyAudio = script->audios[name];
    if (!rpyAudio.audio)
      return false;
    return IRpyInterpreter::Execute_PlayAudio(session->interpreter.GetObject(), autoExecuteNext, channel, rpyAudio, options);
  };
};

struct PauseInstruction : public RpyInstruction
{
  float timeout;

  PauseInstruction(URpyScript *script, FRpyLine *rpyLine, float timeout) : RpyInstruction(script, rpyLine), timeout(timeout){};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::PAUSE; }
  virtual bool Execute(URpySession *session, bool &autoExecuteNext)
  {
    RpyInstruction::Execute(session, autoExecuteNext);
    return IRpyInterpreter::Execute_PauseAudio(session->interpreter.GetObject(), autoExecuteNext, this->timeout);
  };
};

struct ElseInstruction;
//^\s*if\s+[\w\s\.<>=!]+:\s*$
struct IfInstruction : public RpyInstruction
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
  virtual RpyInstruction *GetNext(URpySession *session) override;
};
struct ElseInstruction : public RpyInstruction
{
public:
  bool condition = false;

  ElseInstruction(URpyScript *script, FRpyLine *rpyLine) : RpyInstruction(script, rpyLine){};
  virtual RpyInstructionType Type() const
  {
    return RpyInstructionType::Else;
  }
  virtual RpyInstruction *GetNext(URpySession *session)
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

  virtual bool Execute(URpySession *session, bool &autoExecuteNext)
  {
    this->condition = session->RuntimeData.bools[this->varName];
    this->condition = this->condition ^ this->reverse;
    return true;
  };
};
struct JumpInstruction : public RpyInstruction
{
  FName label;

  JumpInstruction(URpyScript *script, FRpyLine *rpyLine, FName label) : RpyInstruction(script, rpyLine), label(label){};
  virtual bool Compile()
  {
    return this->script->labels.Contains(this->label);
  };
  virtual RpyInstruction *GetNext(URpySession *session)
  {
    return this->script->labels[this->label];
  };
};

struct CallInstruction : public JumpInstruction
{
  CallInstruction(URpyScript *script, FRpyLine *rpyLine, FName name) : JumpInstruction(script, rpyLine, name){};
  virtual RpyInstruction *GetNext(URpySession *session)
  {
    session->callStack.Add(next);
    return JumpInstruction::GetNext(session);
  };
};

struct ReturnInstruction : public RpyInstruction
{
  ReturnInstruction(URpyScript *script, FRpyLine *rpyLine) : RpyInstruction(script, rpyLine){};
  virtual RpyInstruction *GetNext(URpySession *session)
  {
    return session->callStack.Pop();
  };
};

struct StopInstruction : public RpyInstruction
{
  FName channel;
  FRpyAudioOptions options;
  StopInstruction(URpyScript *script, FRpyLine *rpyLine, FName channel, FRpyAudioOptions options) : RpyInstruction(script, rpyLine), channel(channel), options(options){};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::STOP; }
  virtual bool Execute(URpySession *session, bool &autoExecuteNext)
  {
    RpyInstruction::Execute(session, autoExecuteNext);
    return IRpyInterpreter::Execute_StopAudio(session->interpreter.GetObject(), autoExecuteNext, channel, options);
  };
};

struct QueueInstruction : public RpyInstruction
{
  FName channel;
  FName name;
  FRpyAudioOptions options;
  QueueInstruction(URpyScript *script, FRpyLine *rpyLine, FName channel, FName name, FRpyAudioOptions options) : RpyInstruction(script, rpyLine), channel(channel), name(name), options(options){};
  virtual bool Execute(URpySession *session, bool &autoExecuteNext)
  {
    RpyInstruction::Execute(session, autoExecuteNext);
    auto rpyAudio = script->audios[name];
    if (!rpyAudio.audio)
      return false;
    return IRpyInterpreter::Execute_QueueAudio(session->interpreter.GetObject(), autoExecuteNext, channel, rpyAudio, options);
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
      break;
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
RpyInstruction *IfInstruction::GetNext(URpySession *session)
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
