#pragma once

#include "RpyInstructionHelpers.h"
#include "TogetherInstruction.generated.h"

/**
 * <together>: starts every child command at once, then waits until all of those that wait (a show or hide with a
 * transition, a line of dialogue, a pause) have finished. Outside a <together>, each such command finishes before the
 * next one starts.
 */
UCLASS(BlueprintType)
class UTogetherInstruction : public URpyInstruction
{
  GENERATED_BODY()

public:
  /** Children that returned Wait and have not reported completion yet, oldest first. */
  UPROPERTY(Transient)
  TArray<URpyInstruction *> PendingChildren;

  virtual FString ToString() const override { return FormatInstruction(TEXT("together"), FString::Printf(TEXT("commands=%d"), children.Num())); }

  /** Say while a line of dialogue is still waiting, so the statement manager reveals and completes it; else the oldest waiting command's type. */
  virtual EInstructionRunTimeType RunTimeType() const override
  {
    for (const URpyInstruction *Child : PendingChildren)
      if (Child && Child->RunTimeType() == EInstructionRunTimeType::SAY)
        return EInstructionRunTimeType::SAY;
    return PendingChildren.Num() > 0 && PendingChildren[0] ? PendingChildren[0]->RunTimeType() : EInstructionRunTimeType::NONE;
  }

  virtual bool Compile() override
  {
    static const TSet<FName> Allowed = {FName(TEXT("say")), FName(TEXT("show")), FName(TEXT("hide")), FName(TEXT("scene")), FName(TEXT("pause")),
                                        FName(TEXT("play")), FName(TEXT("queue")), FName(TEXT("voice")), FName(TEXT("stop")), FName(TEXT("set"))};
    bool bValid = true;
    for (const URpyInstruction *Child : children)
    {
      if (!Child)
        continue;
      if (!Allowed.Contains(Child->InstructionType) && !FRpyInstructionRegistry::IsRegistered(Child->InstructionType))
      {
        UE_LOG(LogTemp, Error, TEXT("<together> cannot contain <%s>: it holds commands that run at the same time (say, show, hide, scene, pause, play, queue, voice, stop, set and custom commands), not labels, menus, conditions, jumps or nested blocks."),
               *Child->InstructionType.ToString());
        bValid = false;
      }
    }
    if (children.Num() == 0)
      UE_LOG(LogTemp, Warning, TEXT("An empty <together> does nothing."));
    return bValid;
  }

  virtual ERpyInstructionResult Execute(URpySession *session) override
  {
    PendingChildren.Reset();
    bInExecute = true;
    for (URpyInstruction *Child : children)
    {
      if (!Child)
        continue;
      session->executing = Child;
      const ERpyInstructionResult Result = Child->Execute(session);
      Child->LastResult = Result;
      if (Result == ERpyInstructionResult::Wait)
        PendingChildren.Add(Child);
      else if (Result == ERpyInstructionResult::Failed)
        UE_LOG(LogTemp, Error, TEXT("<together>: %s failed; the other commands carry on."), *Child->ToString());
    }
    session->executing = this;
    bInExecute = false;
    return PendingChildren.Num() > 0 ? ERpyInstructionResult::Wait : ERpyInstructionResult::Continue;
  }

  /** The children ran inside Execute, so the script continues after the block. */
  virtual URpyInstruction *GetNext(URpySession *session) override { return next; }

  virtual bool CompleteChild(URpyInstruction *Child) override { return PendingChildren.Remove(Child) > 0; }
  virtual bool CompleteFirstChild() override
  {
    if (PendingChildren.Num() == 0)
      return false;
    PendingChildren.RemoveAt(0);
    return true;
  }
  /** A child that completes while the others are still starting does not end the block early. */
  virtual bool IsWaitComplete() const override { return !bInExecute && PendingChildren.Num() == 0; }
  virtual void Skip(URpySession *session) override
  {
    const TArray<URpyInstruction *> Waiting = PendingChildren;
    for (URpyInstruction *Child : Waiting)
      if (Child && PendingChildren.Contains(Child))
        Child->Skip(session);
  }

private:
  bool bInExecute = false;
};
