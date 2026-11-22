#pragma once

#include "RpyInstructionHelpers.h"
#include "UnsupportedInstruction.generated.h"

UCLASS(BlueprintType)
class UUnsupportedInstruction : public URpyInstruction
{
  GENERATED_BODY()

public:
  virtual FString ToString() const override { return FormatInstruction(TEXT("unsupported"), FString::Printf(TEXT("type=\"%s\""), *InstructionType.ToString())); }
  virtual bool Execute(URpySession *) override
  {
    UE_LOG(LogTemp, Error, TEXT("Unsupported instruction: %s"), *ToString());
    return false;
  }
};
