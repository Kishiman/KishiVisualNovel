#pragma once

#include "C++/RpyInstruction.h"
#include "Interfaces/RpyInterpreter.h"
#include "RpySession.h"

// Labels & Control Flow
struct BlankInstruction : public RpyInstruction
{
  BlankInstruction(URpyScript *script, FRpyLine *rpyLine) : RpyInstruction(script, rpyLine){};
  virtual bool Execute(URpySession *session)
  {
    RpyInstruction::Execute(session);
    return true;
  };
};

struct JumpInstruction : public RpyInstruction
{
  FName name;
  JumpInstruction(URpyScript *script, FRpyLine *rpyLine, FName name) : RpyInstruction(script, rpyLine), name(name){};
  virtual RpyInstruction *GetNext(URpySession *session)
  {
    auto label = script->labels[name];
    if (!label)
    {
      UE_LOG(LogTemp, Error, TEXT("unvalid label name:%s"), (*name.ToString()));
      return nullptr;
    }
    return label;
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

struct SceneInstruction : public RpyInstruction
{
  TMap<FName, FString> params;

  SceneInstruction(URpyScript *script, FRpyLine *rpyLine, TMap<FName, FString> params) : RpyInstruction(script, rpyLine), params(params){};
  virtual bool Execute(URpySession *session)
  {
    RpyInstruction::Execute(session);
    return IRpyInterpreter::Execute_Scene(session->interpreter.GetObject(), this->params);
  };
};

struct ShowInstruction : public RpyInstruction
{
  FName name;
  FName at;
  FName with;

  ShowInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, FName at, FName with) : RpyInstruction(script, rpyLine), name(name), at(at), with(with){};
  virtual bool Execute(URpySession *session)
  {
    RpyInstruction::Execute(session);
    auto rpyImage = script->images[name];
    if (!rpyImage.image)
      return false;
    return IRpyInterpreter::Execute_Show(session->interpreter.GetObject(), rpyImage, at, with);
  };
};

struct HideInstruction : public RpyInstruction
{
  FName name;
  FName at;
  FName with;

  HideInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, FName at, FName with) : RpyInstruction(script, rpyLine), name(name), at(at), with(with){};
  virtual bool Execute(URpySession *session)
  {
    RpyInstruction::Execute(session);
    auto rpyImage = script->images[name];
    if (!rpyImage.image)
      return false;
    return IRpyInterpreter::Execute_Hide(session->interpreter.GetObject(), rpyImage, at, with);
  };
};

struct SayInstruction : public RpyInstruction
{
  FName name;
  FString statement;
  FName with;

  SayInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, FString statement, FName with = "") : RpyInstruction(script, rpyLine), name(name), statement(statement), with(with){};
  virtual bool Execute(URpySession *session)
  {
    RpyInstruction::Execute(session);
    return IRpyInterpreter::Execute_Say(session->interpreter.GetObject(), this->name, this->statement);
  };
};

struct StopInstruction : public RpyInstruction
{
  FName channel;
  FRpyAudioOptions options;
  StopInstruction(URpyScript *script, FRpyLine *rpyLine, FName channel, FRpyAudioOptions options) : RpyInstruction(script, rpyLine), channel(channel), options(options){};
  virtual bool Execute(URpySession *session)
  {
    RpyInstruction::Execute(session);
    return IRpyInterpreter::Execute_StopAudio(session->interpreter.GetObject(), channel, options);
  };
};

struct QueueInstruction : public RpyInstruction
{
  FName channel;
  FName name;
  FRpyAudioOptions options;
  QueueInstruction(URpyScript *script, FRpyLine *rpyLine, FName channel, FName name, FRpyAudioOptions options) : RpyInstruction(script, rpyLine), channel(channel), name(name), options(options){};
  virtual bool Execute(URpySession *session)
  {
    RpyInstruction::Execute(session);
    auto rpyAudio = script->audios[name];
    if (!rpyAudio.audio)
      return false;
    return IRpyInterpreter::Execute_QueueAudio(session->interpreter.GetObject(), channel, rpyAudio, options);
  };
};

struct PlayInstruction : public RpyInstruction
{
  FName channel;
  FName name;
  FRpyAudioOptions options;

  PlayInstruction(URpyScript *script, FRpyLine *rpyLine, FName channel, FName name, FRpyAudioOptions options) : RpyInstruction(script, rpyLine), channel(channel), name(name), options(options){};
  virtual bool Execute(URpySession *session)
  {
    RpyInstruction::Execute(session);
    auto rpyAudio = script->audios[name];
    if (!rpyAudio.audio)
      return false;
    return IRpyInterpreter::Execute_PlayAudio(session->interpreter.GetObject(), channel, rpyAudio, options);
  };
};

struct PauseInstruction : public RpyInstruction
{
  float timeout;

  PauseInstruction(URpyScript *script, FRpyLine *rpyLine, float timeout) : RpyInstruction(script, rpyLine), timeout(timeout){};
  virtual bool Execute(URpySession *session)
  {
    RpyInstruction::Execute(session);
    return IRpyInterpreter::Execute_PauseAudio(session->interpreter.GetObject(), this->timeout);
  };
};
