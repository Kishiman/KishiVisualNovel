#pragma once

#include "RpyInstructionHelpers.h"
#include "ReturnInstruction.generated.h"

UCLASS(BlueprintType)
class UReturnInstruction : public URpyInstruction
{
  GENERATED_BODY()

public:
  virtual FString ToString() const override { return FormatInstruction(TEXT("return")); }
  virtual URpyInstruction *GetNext(URpySession *session)
  {
    if (session->callStack.Num() == 0)
      return nullptr;
    return session->callStack.Pop();
  };
  virtual TArray<URpyInstruction *> GetPossibleNextInstructions() const override
  {
    TArray<URpyInstruction *> Result;
    if (!script) return Result;
    for (URpyInstruction *Instruction : script->instructions)
      if (Instruction && Instruction->IsCallInstruction() && Instruction->next)
        Result.AddUnique(Instruction->next);
    return Result;
  }
  virtual bool IsControlFlowBoundary() const override { return true; }
  virtual TArray<FString> GetPossibleNextInstructionLabels() const override
  {
    TArray<FString> Result;
    const TArray<URpyInstruction *> Successors = GetPossibleNextInstructions();
    for (int32 Index = 0; Index < Successors.Num(); ++Index)
      Result.Add(TEXT("return"));
    return Result;
  }
};
