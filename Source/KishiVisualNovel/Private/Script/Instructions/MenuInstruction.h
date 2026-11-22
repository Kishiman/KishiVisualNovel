#pragma once

#include "RpyInstructionHelpers.h"
#include "ChoiceInstruction.h"
#include "MenuInstruction.generated.h"

UCLASS(BlueprintType)
class UMenuInstruction : public URpyInstruction
{
  GENERATED_BODY()

public:
  virtual FString ToString() const override { return FormatInstruction(TEXT("menu")); }
  UPROPERTY(Transient, VisibleAnywhere, Category = "Instruction")
  TArray<UChoiceInstruction *> choices;
  UPROPERTY(Transient, VisibleAnywhere, Category = "Instruction")
  UChoiceInstruction *selected = nullptr;
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
        choices.Add((UChoiceInstruction *)child);
    }
    return true;
  }
  virtual ERpyInstructionResult Execute(URpySession *session) override
  {
    selected = nullptr;
    TArray<FString> statements;
    session->choiceState.choices.Empty();
    for (auto &choice : choices)
    {
      statements.Add(choice->statement);
      session->choiceState.choices.Add(choice->statement);
    }
    UObject *ChoiceManager = RpyFindManager(session, TEXT("menu"), TEXT("choice manager"), [](UObject *Interpreter)
                                    { return IRpyScriptInterpreter::Execute_GetChoiceManager(Interpreter); });
    if (!ChoiceManager)
      return ERpyInstructionResult::Failed;

    // A menu always waits for the player's choice, so Continue is treated as Wait.
    if (IRpyChoiceManager::Execute_Menu(ChoiceManager, statements) == ERpyInstructionResult::Failed)
      return ERpyInstructionResult::Failed;
    return ERpyInstructionResult::Wait;
  }
  virtual URpyInstruction *GetNext(URpySession *session) override
  {
    // No choice is selected only when the menu failed and the session advances past it.
    return selected ? selected : static_cast<URpyInstruction *>(next);
  }
  virtual TArray<URpyInstruction *> GetPossibleNextInstructions() const override
  {
    TArray<URpyInstruction *> Result;
    for (UChoiceInstruction *Choice : choices)
      if (Choice) Result.Add(Choice);
    return Result;
  }
  virtual bool IsControlFlowBoundary() const override { return true; }
  virtual TArray<FString> GetPossibleNextInstructionLabels() const override
  {
    TArray<FString> Result;
    for (const UChoiceInstruction *Choice : choices)
      if (Choice) Result.Add(Choice->statement);
    return Result;
  }
};
