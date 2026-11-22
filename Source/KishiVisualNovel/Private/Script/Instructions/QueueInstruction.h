#pragma once

#include "RpyInstructionHelpers.h"
#include "QueueInstruction.generated.h"

UCLASS(BlueprintType)
class UQueueInstruction : public URpyInstruction
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instruction")
  FName channel;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instruction")
  FName name;
  virtual FString ToString() const override { return FormatInstruction(TEXT("queue"), FString::Printf(TEXT("channel=\"%s\" asset=\"%s\""), *channel.ToString(), *name.ToString())); }
  virtual ERpyInstructionResult Execute(URpySession *session)
  {
    const FRpyAudioOptions options = AudioOptionsFromProps(Props);
    const FRpyAudio *FoundAudio = script ? script->audios.Find(name) : nullptr;
    if (!FoundAudio || !FoundAudio->audio)
    {
      UE_LOG(LogTemp, Error, TEXT("queue: audio '%s' is not defined in the script or has no sound."), *name.ToString());
      return ERpyInstructionResult::Failed;
    }
    const FRpyAudio rpyAudio = *FoundAudio;
    // TODO handle state
    UObject *AudioManager = RpyFindManager(session, TEXT("queue"), TEXT("audio manager"), [](UObject *Interpreter)
                                    { return IRpyScriptInterpreter::Execute_GetAudioManager(Interpreter); });
    if (!AudioManager)
      return ERpyInstructionResult::Failed;

    return IRpyAudioManager::Execute_QueueAudio(AudioManager, channel, rpyAudio, options);
  };
};
