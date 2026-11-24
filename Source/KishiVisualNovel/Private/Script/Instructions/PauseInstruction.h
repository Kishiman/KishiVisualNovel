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
  virtual ERpyInstructionResult Execute(URpySession *session)
  {
    running = true;
    if (timeInSec > 0)
    {
      // Start the timer with a 5 second delay
      session->GetWorld()->GetTimerManager().SetTimer(
          TimerHandle, [this, session]()
          {
            this->running = false;
            session->CompleteInstruction(this); },
          timeInSec, false);
    }
    // Without a duration, the pause lasts until the player skips it (a click).
    return ERpyInstructionResult::Wait;
  };
  /** A click ends the pause early. */
  virtual void Skip(URpySession *session) override
  {
    if (!running)
      return;
    running = false;
    if (session->GetWorld())
      session->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
    session->CompleteInstruction(this);
  }
  virtual URpyInstruction *GetNext(URpySession *session) override
  {
    if (running == true)
    {
      session->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
    }
    return URpyInstruction::GetNext(session);
  }
};
