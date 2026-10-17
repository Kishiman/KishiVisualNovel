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
  virtual bool Execute(URpySession *session)
  {
    const FRpyAudioOptions options = AudioOptionsFromProps(Props);
    auto rpyAudio = script->audios[name];
    if (!rpyAudio.audio)
      return false;

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
    auto audioManager = IRpyScriptInterpreter::Execute_GetAudioManager(session->interpreter.GetObject());

    return IRpyAudioManager::Execute_PlayAudio(audioManager.GetObject(), channel, rpyAudio, options);
  };
};
