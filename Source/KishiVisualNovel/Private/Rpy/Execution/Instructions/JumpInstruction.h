#pragma once

#include "RpyInstructionHelpers.h"
#include "JumpInstruction.generated.h"

UCLASS(BlueprintType)
class UJumpInstruction : public URpyInstruction
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instruction")
  FName label;
  virtual FString ToString() const override { return FormatInstruction(TEXT("jump"), FString::Printf(TEXT("label=\"%s\""), *label.ToString())); }
  virtual bool Compile()
  {
    return this->script->labels.Contains(this->label);
  };
  virtual URpyInstruction *GetNext(URpySession *session)
  {
    return this->script->labels[this->label];
  };
  virtual TArray<URpyInstruction *> GetPossibleNextInstructions() const override
  {
    TArray<URpyInstruction *> Result;
    if (script)
      if (URpyInstruction * const *Target = script->labels.Find(label))
        if (*Target) Result.Add(*Target);
    return Result;
  }
  virtual bool IsControlFlowBoundary() const override { return true; }
  virtual TArray<FString> GetPossibleNextInstructionLabels() const override
  {
    return {FString::Printf(TEXT("jump %s"), *label.ToString())};
  }
};
