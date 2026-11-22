#pragma once

#include "RpyInstructionHelpers.h"
#include "PlayInstruction.generated.h"

UCLASS(BlueprintType)
class UPlayInstruction : public URpyInstruction
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instruction")
  FName channel;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instruction")
  FName name;
  virtual FString ToString() const override { return FormatInstruction(TEXT("play"), FString::Printf(TEXT("channel=\"%s\" asset=\"%s\""), *channel.ToString(), *name.ToString())); }
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::PLAY; }
  virtual ERpyInstructionResult Execute(URpySession *session)
  {
    const FRpyAudioOptions options = AudioOptionsFromProps(Props);
    const FRpyAudio *FoundAudio = script ? script->audios.Find(name) : nullptr;
    if (!FoundAudio || !FoundAudio->audio)
    {
      UE_LOG(LogTemp, Error, TEXT("play: audio '%s' is not defined in the script or has no sound."), *name.ToString());
      return ERpyInstructionResult::Failed;
    }
    const FRpyAudio rpyAudio = *FoundAudio;

    auto existing = session->audioState.audioStates.FindByPredicate([&](const FRpyAudioPlayState &state)
                                                                    { return state.channel == channel; });
    if (existing)
    {
      existing->rpyAudio = rpyAudio;
      existing->options = options;
    }
    else
    {
      session->audioState.audioStates.Add(FRpyAudioPlayState{channel, rpyAudio, options});
    }
    UObject *AudioManager = RpyFindManager(session, TEXT("play"), TEXT("audio manager"), [](UObject *Interpreter)
                                    { return IRpyScriptInterpreter::Execute_GetAudioManager(Interpreter); });
    if (!AudioManager)
      return ERpyInstructionResult::Failed;

    return IRpyAudioManager::Execute_PlayAudio(AudioManager, channel, rpyAudio, options);
  };
};
