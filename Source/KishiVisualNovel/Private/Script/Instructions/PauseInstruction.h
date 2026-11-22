#pragma once

#include "RpyInstructionHelpers.h"
#include "PauseInstruction.generated.h"

UCLASS(BlueprintType)
class UPauseInstruction : public URpyInstruction
{
  GENERATED_BODY()

public:
  FTimerHandle TimerHandle;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instruction")
  float timeInSec = 0;
  virtual FString ToString() const override { return FormatInstruction(TEXT("pause"), FString::Printf(TEXT("duration=\"%g\""), timeInSec)); }
  UPROPERTY(Transient)
  bool running = false;
  virtual bool Execute(URpySession *session)
  {
    running = true;
    if (timeInSec > 0)
    {
      // Start the timer with a 5 second delay
      session->GetWorld()->GetTimerManager().SetTimer(
          TimerHandle, [this, session]()
          {
            this->running = false;
            session->RunNext(); },
          timeInSec, false);
    }
    return false;
  };
  virtual URpyInstruction *GetNext(URpySession *session) override
  {
    if (running == true)
    {
      session->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
    }
    return URpyInstruction::GetNext(session);
  }
};
