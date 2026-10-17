#pragma once

#include "RpyInstructionHelpers.h"
#include "AssignInstruction.generated.h"

UCLASS(BlueprintType)
class UAssignInstruction : public URpyInstruction
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instruction")
  FName varName;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instruction")
  FString value;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instruction")
  FString valueType;
  virtual FString ToString() const override { return FormatInstruction(TEXT("set"), FString::Printf(TEXT("name=\"%s\" value=\"%s\""), *varName.ToString(), *value)); }

  void SetValue(URpySession *session);
  virtual bool Execute(URpySession *session) override
  {
    SetValue(session);
    return true;
  }
};
