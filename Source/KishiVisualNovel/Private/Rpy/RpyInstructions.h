#pragma once

#include "Lib/ArrayLib.h"
#include "Rpy/RpyInstruction.h"
#include "Interfaces/RpyScriptInterpreter.h"

#include "Interfaces/ScriptInterpreterManagers/RpyAudioManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyChoiceManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpySceneManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyShowManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyStatementManager.h"
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
void AssignInstruction<bool>::SetValue(URpySession *session) { session->runtimeData.bools.Add(varName, value); }
void AssignInstruction<FName>::SetValue(URpySession *session) { session->runtimeData.names.Add(varName, value); }
void AssignInstruction<FString>::SetValue(URpySession *session) { session->runtimeData.strings.Add(varName, value); }
void AssignInstruction<int>::SetValue(URpySession *session) { session->runtimeData.ints.Add(varName, value); }
void AssignInstruction<float>::SetValue(URpySession *session) { session->runtimeData.floats.Add(varName, value); }

struct SceneInstruction : public RpyInstruction
{
  FName name;
  FRpySceneOptions options;

  SceneInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, FRpySceneOptions options) : RpyInstruction(script, rpyLine), name(name), options(options) {};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::SCENE; }
  virtual bool Execute(URpySession *session)
  {
    FRpyImage rpyImage;
    if (name != "_")
    {
      auto _rpyImage = script->images.Find(name);
      if (!_rpyImage)
        return false;
      rpyImage = *_rpyImage;
    }
    session->sceneState.rpyImage = rpyImage;
    session->sceneState.options = options;
    auto sceneManager = IRpyScriptInterpreter::Execute_GetSceneManager(session->interpreter.GetObject());
    return IRpySceneManager::Execute_Scene(sceneManager.GetObject(), rpyImage, options);
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

    auto existing = session->showState.imageStates.FindByPredicate([&](const FRpyShowImageState &state)
                                                                   { return state.name == name; });
    if (existing)
    {
      existing->rpyImage = *rpyImage;
      existing->options = options;
    }
    else
    {
      session->showState.imageStates.Add(FRpyShowImageState{name, *rpyImage, options});
    }

    auto showManager = IRpyScriptInterpreter::Execute_GetShowManager(session->interpreter.GetObject());

    return IRpyShowManager::Execute_Show(showManager.GetObject(), *rpyImage, options);
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

    auto existing = ArrayLib::FindIndexByPredicate(session->showState.imageStates, [&](const FRpyShowImageState &state)
                                                   { return state.name == tag; });
    if (existing != INDEX_NONE)
    {
      session->showState.imageStates.RemoveAt(existing);
    }
    auto showManager = IRpyScriptInterpreter::Execute_GetShowManager(session->interpreter.GetObject());

    return IRpyShowManager::Execute_Hide(showManager.GetObject(), tag, options);
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
    auto statementManager = IRpyScriptInterpreter::Execute_GetStatementManager(session->interpreter.GetObject());

    session->statementState.name = this->name;
    session->statementState.statement = this->statement;
    session->statementState.with = this->with;

    return IRpyStatementManager::Execute_Say(statementManager.GetObject(), this->name, this->statement);
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

    auto existing = session->audioState.audioStates.FindByPredicate([&](const FRpyAudioPlayState &state)
                                                                    { return state.channel == channel; });
    if (existing)
    {
      existing->rpyAudio = rpyAudio;
      existing->options = options;
    }
    else
    {
      session->audioState.audioStates.Add(FRpyAudioPlayState{channel, rpyAudio, options});
    }
    auto audioManager = IRpyScriptInterpreter::Execute_GetAudioManager(session->interpreter.GetObject());

    return IRpyAudioManager::Execute_PlayAudio(audioManager.GetObject(), channel, rpyAudio, options);
  };
};

struct PauseAudioInstruction : public RpyInstruction
{
  float timeout;

  PauseAudioInstruction(URpyScript *script, FRpyLine *rpyLine, float timeout) : RpyInstruction(script, rpyLine), timeout(timeout) {};
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::PAUSE; }
  virtual bool Execute(URpySession *session)
  {
    session->audioState.audioStates.Empty();
    auto audioManager = IRpyScriptInterpreter::Execute_GetAudioManager(session->interpreter.GetObject());

    return IRpyAudioManager::Execute_PauseAudio(audioManager.GetObject(), this->timeout);
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
    if (!session->runtimeData.bools.Contains(this->varName))
    {
      UE_LOG(LogTemp, Fatal, TEXT("bool variable name %s does not exist in Runtime data"), *this->varName.ToString());
      return false;
    }
    this->condition = session->runtimeData.bools[this->varName];
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
    session->choiceState.choices.Empty();
    for (auto &choice : choices)
    {
      statements.Add(choice->statement);
      session->choiceState.choices.Add(choice->statement);
    }
    auto choiceManager = IRpyScriptInterpreter::Execute_GetChoiceManager(session->interpreter.GetObject());

    IRpyChoiceManager::Execute_Menu(choiceManager.GetObject(), statements);
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
    auto existing = ArrayLib::FindIndexByPredicate(session->audioState.audioStates, [&](const FRpyAudioPlayState &state)
                                                   { return state.channel == channel; });
    if (existing != INDEX_NONE)
    {
      session->audioState.audioStates.RemoveAt(existing);
    }
    auto audioManager = IRpyScriptInterpreter::Execute_GetAudioManager(session->interpreter.GetObject());

    return IRpyAudioManager::Execute_StopAudio(audioManager.GetObject(), channel, options);
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
    // TODO handle state
    auto audioManager = IRpyScriptInterpreter::Execute_GetAudioManager(session->interpreter.GetObject());

    return IRpyAudioManager::Execute_QueueAudio(audioManager.GetObject(), channel, rpyAudio, options);
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
