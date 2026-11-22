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
  virtual ERpyInstructionResult Execute(URpySession *session)
  {
    const FRpyAudioOptions options = AudioOptionsFromProps(Props);
    auto existing = ArrayLib::FindIndexByPredicate(session->audioState.audioStates, [&](const FRpyAudioPlayState &state)
                                                   { return state.channel == channel; });
    if (existing != INDEX_NONE)
    {
      session->audioState.audioStates.RemoveAt(existing);
    }
    UObject *AudioManager = RpyFindManager(session, TEXT("stop"), TEXT("audio manager"), [](UObject *Interpreter)
                                    { return IRpyScriptInterpreter::Execute_GetAudioManager(Interpreter); });
    if (!AudioManager)
      return ERpyInstructionResult::Failed;

    return IRpyAudioManager::Execute_StopAudio(AudioManager, channel, options);
  };
};
