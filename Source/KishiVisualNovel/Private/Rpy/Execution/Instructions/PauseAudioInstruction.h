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
  virtual bool Execute(URpySession *session)
  {
    session->audioState.audioStates.Empty();
    auto audioManager = IRpyScriptInterpreter::Execute_GetAudioManager(session->interpreter.GetObject());

    return IRpyAudioManager::Execute_PauseAudio(audioManager.GetObject(), this->timeout);
  };
};

//^\s*if\s+[\w\s\.<>=!]+:\s*$
