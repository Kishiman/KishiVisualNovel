#pragma once

#include "RpyInstructionHelpers.h"
#include "StopInstruction.generated.h"

UCLASS(BlueprintType)
class UStopInstruction : public URpyInstruction
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instruction")
  FName channel;
  virtual FString ToString() const override { return FormatInstruction(TEXT("stop"), FString::Printf(TEXT("channel=\"%s\""), *channel.ToString())); }
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::STOP; }
  virtual bool Execute(URpySession *session)
  {
    const FRpyAudioOptions options = AudioOptionsFromProps(Props);
    auto existing = ArrayLib::FindIndexByPredicate(session->audioState.audioStates, [&](const FRpyAudioPlayState &state)
                                                   { return state.channel == channel; });
    if (existing != INDEX_NONE)
    {
      session->audioState.audioStates.RemoveAt(existing);
    }
    auto audioManager = IRpyScriptInterpreter::Execute_GetAudioManager(session->interpreter.GetObject());

    return IRpyAudioManager::Execute_StopAudio(audioManager.GetObject(), channel, options);
  };
};
