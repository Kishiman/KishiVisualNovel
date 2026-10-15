#pragma once

#include "RpyInstructionHelpers.h"

struct SayInstruction : public RpyInstruction
{
  FName name;
  FString statement;

  SayInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, FString statement, FName with = "") : RpyInstruction(script, rpyLine), name(name), statement(statement)
  {
    if (!with.IsNone())
      Props.SetString(FName(TEXT("with")), with.ToString());
  };
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::SAY; }
  virtual bool Execute(URpySession *session)
  {
    auto statementManager = IRpyScriptInterpreter::Execute_GetStatementManager(session->interpreter.GetObject());
    FString RenderedStatement = this->statement;
    FTaggedString TaggedStatement;
    if (UStringUtils::StringToTaggedString(this->statement, TaggedStatement))
    {
      RenderedStatement = UStringUtils::GetTaggedString(TaggedStatement);
    }

    FName DisplayName = this->name;
    if (const FRpyCharacter *Character = script->characters.Find(this->name))
    {
      DisplayName = Character->name;
    }
    session->statementState.name = DisplayName;
    session->statementState.statement = RenderedStatement;
    FString With;
    if (Props.GetString(FName(TEXT("with")), With))
      session->statementState.with = FName(*With);

    return IRpyStatementManager::Execute_Say(statementManager.GetObject(), DisplayName, RenderedStatement);
  };
};
