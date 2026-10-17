#pragma once

#include "RpyInstructionHelpers.h"
#include "JumpInstruction.h"
#include "CallInstruction.generated.h"

UCLASS(BlueprintType)
class UCallInstruction : public UJumpInstruction
{
  GENERATED_BODY()

public:
  virtual FString ToString() const override { return FormatInstruction(TEXT("call"), FString::Printf(TEXT("label=\"%s\""), *label.ToString())); }
  virtual bool IsCallInstruction() const override { return true; }
  virtual TArray<FString> GetPossibleNextInstructionLabels() const override
  {
    return {FString::Printf(TEXT("call %s"), *label.ToString())};
  }
  virtual URpyInstruction *GetNext(URpySession *session)
  {
    session->callStack.Add(next);
    return UJumpInstruction::GetNext(session);
  };
};
