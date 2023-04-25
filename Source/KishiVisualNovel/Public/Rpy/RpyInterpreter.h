#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Rpy/RpyScript.h"

#include "RpyInterpreter.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API URpyInterpreter : public UInterface
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyInterpreter
{
  GENERATED_BODY()
public:
  // instructions
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool Say(FName name, const FString &statement);
  virtual bool Say_Implementation(FName name, const FString &statement) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool Scene(FName name, FRpyImageOptions options);
  virtual bool Scene_Implementation(FName name, FRpyImageOptions options) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool Show(FRpyImage rpyImage, FName at, FName with);
  virtual bool Show_Implementation(FRpyImage rpyImage, FName at, FName with) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool Hide(FRpyImage rpyImage, FName at, FName with);
  virtual bool Hide_Implementation(FRpyImage rpyImage, FName at, FName with) = 0;

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

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool Menu(const FString &statement, const TArray<FString> &choices);
  virtual bool Menu_Implementation(const FString &statement, const TArray<FString> &choices) = 0;

  // Handlers Interface

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void Choose(int idx);
  virtual void Choose_Implementation(int idx) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool AutoRunNext() const;
  virtual bool AutoRunNext_Implementation() const = 0;
};

UCLASS()
class KISHIVISUALNOVEL_API URpyInterpreterLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
protected:
private:
};
