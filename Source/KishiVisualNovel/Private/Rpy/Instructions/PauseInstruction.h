#pragma once

#include "RpyInstructionHelpers.h"

struct PauseInstruction : public RpyInstruction
{
  FTimerHandle TimerHandle;
  float timeInSec = 0;
  bool running = false;
  PauseInstruction(URpyScript *script, FRpyLine *rpyLine, float timeInSec) : RpyInstruction(script, rpyLine), timeInSec(timeInSec) {};
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
  virtual RpyInstruction *GetNext(URpySession *session) override
  {
    if (running == true)
    {
      session->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
    }
    return RpyInstruction::GetNext(session);
  }
};
