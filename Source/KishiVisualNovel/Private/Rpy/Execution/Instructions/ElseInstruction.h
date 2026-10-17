#pragma once

#include "RpyInstructionHelpers.h"
#include "ElseInstruction.generated.h"

UCLASS(BlueprintType)
class UElseInstruction : public URpyInstruction
{
  GENERATED_BODY()

public:
  virtual FString ToString() const override { return FormatInstruction(TEXT("else")); }
  virtual RpyInstructionType Type() const { return RpyInstructionType::Else; }
};
