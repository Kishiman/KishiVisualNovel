#pragma once

#include "RpyInstructionHelpers.h"
#include "IfInstruction.generated.h"

UCLASS(BlueprintType)
class UIfInstruction : public URpyInstruction
{
  GENERATED_BODY()

protected:
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Instruction")
  bool condition = true;
  UPROPERTY(Transient)
  UElseInstruction *elseInstruction = nullptr;

public:
  virtual FString ToString() const override { return FormatInstruction(TEXT("if")); }
  virtual RpyInstructionType Type() const
  {
    return RpyInstructionType::If;
  }
  virtual bool Compile() override;
  virtual URpyInstruction *GetNext(URpySession *session) override;
  virtual TArray<URpyInstruction *> GetPossibleNextInstructions() const override;
  virtual TArray<FString> GetPossibleNextInstructionLabels() const override
  {
    TArray<FString> Result;
    const TArray<URpyInstruction *> Successors = GetPossibleNextInstructions();
    if (children.Num() > 0 && children[0]) Result.Add(TEXT("true"));
    if (Successors.Num() > Result.Num()) Result.Add(TEXT("false"));
    return Result;
  }
  virtual bool IsControlFlowBoundary() const override { return true; }
};
