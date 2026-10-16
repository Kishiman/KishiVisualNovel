#pragma once

#include "RpyInstructionHelpers.h"
#include "SayInstruction.generated.h"

UCLASS(BlueprintType)
class USayInstruction : public URpyInstruction
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instruction")
  FName name;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instruction")
  FString statement;
  virtual FString ToString() const override { return FormatInstruction(TEXT("say"), FString::Printf(TEXT("character=\"%s\" %s"), *name.ToString(), *statement)); }
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
