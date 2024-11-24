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
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool PauseAudio(float timeout);
  virtual bool PauseAudio_Implementation(float timeout) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool StopAudio(FName channel, FRpyAudioOptions options);
  virtual bool StopAudio_Implementation(FName channel, FRpyAudioOptions options) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool QueueAudio(FName channel, FRpyAudio audio, FRpyAudioOptions options);
  virtual bool QueueAudio_Implementation(FName channel, FRpyAudio audio, FRpyAudioOptions options) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool PlayAudio(FName channel, FRpyAudio audio, FRpyAudioOptions options);
  virtual bool PlayAudio_Implementation(FName channel, FRpyAudio audio, FRpyAudioOptions options) = 0;
};

// UCLASS()
// class KISHIVISUALNOVEL_API URpyAudioManagerLibrary : public UBlueprintFunctionLibrary
// {
//   GENERATED_BODY()
// public:
// protected:
// private:
// };
