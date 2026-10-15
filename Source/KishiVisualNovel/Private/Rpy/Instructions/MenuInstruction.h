#pragma once

#include "RpyInstructionHelpers.h"
#include "ChoiceInstruction.h"

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
