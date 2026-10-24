#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Interfaces/RpyScriptInterpreterBaseManager.h"

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

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyAudioManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  bool PauseAudio(float timeout);
  virtual bool PauseAudio_Implementation(float timeout) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyAudioManager::PauseAudio")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyAudioManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  bool StopAudio(FName channel, FRpyAudioOptions options);
  virtual bool StopAudio_Implementation(FName channel, FRpyAudioOptions options) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyAudioManager::StopAudio")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyAudioManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  bool QueueAudio(FName channel, FRpyAudio audio, FRpyAudioOptions options);
  virtual bool QueueAudio_Implementation(FName channel, FRpyAudio audio, FRpyAudioOptions options) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyAudioManager::QueueAudio")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyAudioManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  bool PlayAudio(FName channel, FRpyAudio audio, FRpyAudioOptions options);
  virtual bool PlayAudio_Implementation(FName channel, FRpyAudio audio, FRpyAudioOptions options) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyAudioManager::PlayAudio")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyAudioManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  TArray<FRpyAudioManagerState> GetCurrentAudioStates();
  virtual TArray<FRpyAudioManagerState> GetCurrentAudioStates_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyAudioManager::GetCurrentAudioStates")); return {}; }
};

// UCLASS()
// class KISHIVISUALNOVEL_API URpyAudioManagerLibrary : public UBlueprintFunctionLibrary
// {
//   GENERATED_BODY()
// public:
// protected:
// private:
// };
