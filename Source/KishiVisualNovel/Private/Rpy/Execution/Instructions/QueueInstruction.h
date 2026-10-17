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
  virtual bool Execute(URpySession *session)
  {
    const FRpyAudioOptions options = AudioOptionsFromProps(Props);
    auto rpyAudio = script->audios[name];
    if (!rpyAudio.audio)
      return false;
    // TODO handle state
    auto audioManager = IRpyScriptInterpreter::Execute_GetAudioManager(session->interpreter.GetObject());

    return IRpyAudioManager::Execute_QueueAudio(audioManager.GetObject(), channel, rpyAudio, options);
  };
};
