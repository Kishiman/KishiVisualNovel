#pragma once

#include "Rpy/RpyInstruction.h"
#include "Rpy/RpyInterpreter.h"
#include "Rpy/RpySession.h"

struct IfInstruction;
struct ElseInstruction;
struct MenuInstruction;
struct ChoiceInstruction;

// Labels & Control Flow
struct LabelInstruction : public RpyInstruction
{
  LabelInstruction(URpyScript *script, FRpyLine *rpyLine) : RpyInstruction(script, rpyLine) {};
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
struct PauseInstruction : public RpyInstruction
{
  FTimerHandle TimerHandle;
  float timeInSec = 0;
  bool running = false;
  PauseInstruction(URpyScript *script, FRpyLine *rpyLine, float timeInSec) : RpyInstruction(script, rpyLine), timeInSec(timeInSec) {};
  virtual bool Execute(URpySession *session)
  {
    running = true;
    if (timeInSec > 0)
    {
      // Start the timer with a 5 second delay
      session->GetWorld()->GetTimerManager().SetTimer(
          TimerHandle, [this, session]()
          {
            this->running = false;
            session->RunNext(); },
          timeInSec, false);
    }
    return false;
  };
  virtual RpyInstruction *GetNext(URpySession *session) override
  {
    if (running == true)
    {
      session->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
    }
    return RpyInstruction::GetNext(session);
  }
};
template <typename T>
struct AssignInstruction : public RpyInstruction
{
  FName varName;
  T value;

  AssignInstruction(URpyScript *script, FRpyLine *rpyLine, FName varName, T value) : RpyInstruction(script, rpyLine), varName(varName), value(value) {};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::NONE; }
  void SetValue(URpySession *session);
  virtual bool Execute(URpySession *session)
  {
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
  FName name;
  FRpySceneOptions options;

  SceneInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, FRpySceneOptions options) : RpyInstruction(script, rpyLine), name(name), options(options) {};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::SCENE; }
  virtual bool Execute(URpySession *session)
  {
    auto rpyImage = script->images.Find(name);
    if (!rpyImage)
      return false;
    return IRpyInterpreter::Execute_Scene(session->interpreter.GetObject(), *rpyImage, options);
  };
};

struct ShowInstruction : public RpyInstruction
{
  FName name;
  FRpySceneOptions options;

  ShowInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, FRpySceneOptions options) : RpyInstruction(script, rpyLine), name(name), options(options) {};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::SHOW; }
  virtual bool Execute(URpySession *session)
  {
    auto rpyImage = script->images.Find(name);
    if (!rpyImage)
      return false;
    return IRpyInterpreter::Execute_Show(session->interpreter.GetObject(), *rpyImage, options);
  };
};
struct ShowLayeredInstruction : public RpyInstruction
{
  FName name;
  FString attribute;
  FRpySceneOptions options;

  ShowLayeredInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, FString attribute, FRpySceneOptions options) : RpyInstruction(script, rpyLine), name(name), attribute(attribute), options(options) {};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::SHOW; }
  virtual bool Execute(URpySession *session)
  {
    auto rpyLayeredImage = script->layeredImages.Find(name);
    if (!rpyLayeredImage)
      return false;
    return IRpyInterpreter::Execute_ShowLayeredImage(session->interpreter.GetObject(), *rpyLayeredImage, attribute, options);
  };
};

struct HideInstruction : public RpyInstruction
{
  FName tag;
  FRpySceneOptions options;

  HideInstruction(URpyScript *script, FRpyLine *rpyLine, FName tag, FRpySceneOptions options) : RpyInstruction(script, rpyLine), tag(tag), options(options) {};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::HIDE; }
  virtual bool Execute(URpySession *session)
  {
    return IRpyInterpreter::Execute_Hide(session->interpreter.GetObject(), tag, options);
  };
};

struct SayInstruction : public RpyInstruction
{
  FName name;
  FString statement;
  FName with;

  SayInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, FString statement, FName with = "") : RpyInstruction(script, rpyLine), name(name), statement(statement), with(with) {};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::SAY; }
  virtual bool Execute(URpySession *session)
  {
    return IRpyInterpreter::Execute_Say(session->interpreter.GetObject(), this->name, this->statement);
  };
};
struct PlayInstruction : public RpyInstruction
{
  FName channel;
  FName name;
  FRpyAudioOptions options;

  PlayInstruction(URpyScript *script, FRpyLine *rpyLine, FName channel, FName name, FRpyAudioOptions options) : RpyInstruction(script, rpyLine), channel(channel), name(name), options(options) {};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::PLAY; }
  virtual bool Execute(URpySession *session)
  {
    auto rpyAudio = script->audios[name];
    if (!rpyAudio.audio)
      return false;
    return IRpyInterpreter::Execute_PlayAudio(session->interpreter.GetObject(), channel, rpyAudio, options);
  };
};

struct PauseAudioInstruction : public RpyInstruction
{
  float timeout;

  PauseAudioInstruction(URpyScript *script, FRpyLine *rpyLine, float timeout) : RpyInstruction(script, rpyLine), timeout(timeout) {};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::PAUSE; }
  virtual bool Execute(URpySession *session)
  {
    return IRpyInterpreter::Execute_PauseAudio(session->interpreter.GetObject(), this->timeout);
  };
};

//^\s*if\s+[\w\s\.<>=!]+:\s*$
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
struct ElseInstruction : public RpyInstruction
{
public:
  ElseInstruction(URpyScript *script, FRpyLine *rpyLine) : RpyInstruction(script, rpyLine) {};
  virtual RpyInstructionType Type() const { return RpyInstructionType::Else; }
};

class IfBoolInstruction : public IfInstruction
{
public:
  FName varName;
  bool reverse;

  IfBoolInstruction(URpyScript *script, FRpyLine *rpyLine, FName varName, bool reverse) : IfInstruction(script, rpyLine), varName(varName), reverse(reverse) {};

  virtual bool Execute(URpySession *session)
  {
    if (!session->RuntimeData.bools.Contains(this->varName))
    {
      UE_LOG(LogTemp, Fatal, TEXT("bool variable name %s does not exist in Runtime data"), *this->varName.ToString());
      return false;
    }
    this->condition = session->RuntimeData.bools[this->varName];
    this->condition = this->condition ^ this->reverse;
    return true;
  };
};
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

struct MenuInstruction : public RpyInstruction
{
  TArray<ChoiceInstruction *> choices;
  ChoiceInstruction *selected = nullptr;
  MenuInstruction(URpyScript *script, FRpyLine *rpyLine) : RpyInstruction(script, rpyLine) {};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::MENU; }
  virtual RpyInstructionType Type() const
  {
    return RpyInstructionType::Menu;
  }
  virtual bool Compile() override
  {
    for (auto &child : children)
    {
      if (child->isOfType(RpyInstructionType::Choice))
        choices.Add((ChoiceInstruction *)child);
    }
    return true;
  }
  virtual bool Execute(URpySession *session) override
  {
    TArray<FString> statements;
    for (auto &choice : choices)
    {
      statements.Add(choice->statement);
    }
    IRpyInterpreter::Execute_Menu(session->interpreter.GetObject(), statements);
    return false;
  }
  virtual RpyInstruction *GetNext(URpySession *session) override
  {

    return selected;
  }
};
bool ChoiceInstruction::Compile()
{
  if (!parent->isOfType(RpyInstructionType::Menu))
  {
    UE_LOG(LogTemp, Error, TEXT("ChoiceInstruction not directly under MenuInstruction"));
    return false;
  }
  MenuInstruction *menu = (MenuInstruction *)(this->parent);
  if (children.Num() == 0)
  {
    UE_LOG(LogTemp, Error, TEXT("ChoiceInstruction has no children"));
    return false;
  }
  if (menu->choices.Num() == 0)
  {
    UE_LOG(LogTemp, Error, TEXT("MenuInstruction has no choices"));
    return false;
  }
  children.Last()->next = menu->choices.Last()->next;
  return true;
};

struct JumpInstruction : public RpyInstruction
{
  FName label;

  JumpInstruction(URpyScript *script, FRpyLine *rpyLine, FName label) : RpyInstruction(script, rpyLine), label(label) {};
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
  CallInstruction(URpyScript *script, FRpyLine *rpyLine, FName name) : JumpInstruction(script, rpyLine, name) {};
  virtual RpyInstruction *GetNext(URpySession *session)
  {
    session->callStack.Add(next);
    return JumpInstruction::GetNext(session);
  };
};

struct ReturnInstruction : public RpyInstruction
{
  ReturnInstruction(URpyScript *script, FRpyLine *rpyLine) : RpyInstruction(script, rpyLine) {};
  virtual RpyInstruction *GetNext(URpySession *session)
  {
    if (session->callStack.Num() == 0)
      return nullptr;
    return session->callStack.Pop();
  };
};

struct StopInstruction : public RpyInstruction
{
  FName channel;
  FRpyAudioOptions options;
  StopInstruction(URpyScript *script, FRpyLine *rpyLine, FName channel, FRpyAudioOptions options) : RpyInstruction(script, rpyLine), channel(channel), options(options) {};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::STOP; }
  virtual bool Execute(URpySession *session)
  {
    return IRpyInterpreter::Execute_StopAudio(session->interpreter.GetObject(), channel, options);
  };
};

struct QueueInstruction : public RpyInstruction
{
  FName channel;
  FName name;
  FRpyAudioOptions options;
  QueueInstruction(URpyScript *script, FRpyLine *rpyLine, FName channel, FName name, FRpyAudioOptions options) : RpyInstruction(script, rpyLine), channel(channel), name(name), options(options) {};
  virtual bool Execute(URpySession *session)
  {
    auto rpyAudio = script->audios[name];
    if (!rpyAudio.audio)
      return false;
    return IRpyInterpreter::Execute_QueueAudio(session->interpreter.GetObject(), channel, rpyAudio, options);
  };
};

bool IfInstruction::Compile()
{
  RpyInstruction *current = this;
  while (current->next)
  {
    if (current->next->isOfType(RpyInstructionType::Else) && current->next->rpyLine->tabs == this->rpyLine->tabs)
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
    return this->children[0];
  else if (this->elseInstruction)
    return this->elseInstruction;
  return this->next;
};
