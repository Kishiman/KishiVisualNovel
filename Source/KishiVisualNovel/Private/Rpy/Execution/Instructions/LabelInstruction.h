#pragma once

#include "RpyInstructionHelpers.h"
#include "LabelInstruction.generated.h"

UCLASS(BlueprintType)
class ULabelInstruction : public URpyInstruction
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instruction")
  FName name;
  virtual FString ToString() const override { return FormatInstruction(TEXT("label"), FString::Printf(TEXT("name=\"%s\""), *name.ToString())); }
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
