#pragma once

#include "RpyInstructionHelpers.h"
#include "PauseAudioInstruction.generated.h"

UCLASS(BlueprintType)
class UPauseAudioInstruction : public URpyInstruction
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instruction")
  float timeout;
  virtual FString ToString() const override { return FormatInstruction(TEXT("pauseAudio"), FString::Printf(TEXT("duration=\"%g\""), timeout)); }
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::PAUSE; }
  virtual ERpyInstructionResult Execute(URpySession *session)
  {
    session->audioState.audioStates.Empty();
    UObject *AudioManager = RpyFindManager(session, TEXT("pause audio"), TEXT("audio manager"), [](UObject *Interpreter)
                                    { return IRpyScriptInterpreter::Execute_GetAudioManager(Interpreter); });
    if (!AudioManager)
      return ERpyInstructionResult::Failed;

    return IRpyAudioManager::Execute_PauseAudio(AudioManager, this->timeout);
  };
};

//^\s*if\s+[\w\s\.<>=!]+:\s*$
