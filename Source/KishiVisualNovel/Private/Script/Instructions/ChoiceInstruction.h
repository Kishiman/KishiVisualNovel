#pragma once

#include "RpyInstructionHelpers.h"
#include "ChoiceInstruction.generated.h"

UCLASS(BlueprintType)
class UChoiceInstruction : public URpyInstruction
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instruction")
  FString statement;
  virtual FString ToString() const override { return FormatInstruction(TEXT("choice"), FString::Printf(TEXT("text=\"%s\""), *statement)); }
  virtual RpyInstructionType Type() const
  {
    return RpyInstructionType::Choice;
  }
  virtual bool Compile() override;
};
