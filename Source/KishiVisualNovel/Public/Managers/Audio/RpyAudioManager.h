#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Player/RpyScriptInterpreterBaseManager.h"
#include "Script/RpyScriptTypes.h"

#include "RpyAudioManager.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API URpyAudioManager : public URpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyAudioManager : public IRpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
public:
  // get state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyAudioManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  FRpyAudioManagerState GetAudioManagerState() const;
  virtual FRpyAudioManagerState GetAudioManagerState_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyAudioManager::GetAudioManagerState")); return {}; }

  // set state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyAudioManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  void SetAudioManagerState(const FRpyAudioManagerState &State);
  virtual void SetAudioManagerState_Implementation(const FRpyAudioManagerState &State) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyAudioManager::SetAudioManagerState")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyAudioManager|Required", meta = (ToolTip = "Required: pause every channel, fading out over the timeout. Return Continue, or Failed, after logging why, if audio cannot be paused."))
  ERpyInstructionResult PauseAudio(float timeout);
  virtual ERpyInstructionResult PauseAudio_Implementation(float timeout) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyAudioManager::PauseAudio")); return ERpyInstructionResult::Failed; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyAudioManager|Required", meta = (ToolTip = "Required: stop the channel, fading out as the options say. Return Continue, or Failed, after logging why, if the channel cannot be stopped."))
  ERpyInstructionResult StopAudio(FName channel, FRpyAudioOptions options);
  virtual ERpyInstructionResult StopAudio_Implementation(FName channel, FRpyAudioOptions options) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyAudioManager::StopAudio")); return ERpyInstructionResult::Failed; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyAudioManager|Required", meta = (ToolTip = "Required: queue the sound on the channel after what is playing. Return Continue, or Failed, after logging why, if the channel or sound is missing."))
  ERpyInstructionResult QueueAudio(FName channel, FRpyAudio audio, FRpyAudioOptions options);
  virtual ERpyInstructionResult QueueAudio_Implementation(FName channel, FRpyAudio audio, FRpyAudioOptions options) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyAudioManager::QueueAudio")); return ERpyInstructionResult::Failed; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyAudioManager|Required", meta = (ToolTip = "Required: play the sound on the channel, replacing what is playing there. Return Continue, or Failed, after logging why, if the channel or sound is missing."))
  ERpyInstructionResult PlayAudio(FName channel, FRpyAudio audio, FRpyAudioOptions options);
  virtual ERpyInstructionResult PlayAudio_Implementation(FName channel, FRpyAudio audio, FRpyAudioOptions options) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyAudioManager::PlayAudio")); return ERpyInstructionResult::Failed; }
};

// UCLASS()
// class KISHIVISUALNOVEL_API URpyAudioManagerLibrary : public UBlueprintFunctionLibrary
// {
//   GENERATED_BODY()
// public:
// protected:
// private:
// };
