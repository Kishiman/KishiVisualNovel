#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "DataAssets/RpyScript.h"

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
  bool Say(bool &autoExecuteNext, FName name, const FString &statement);
  virtual bool Say_Implementation(bool &autoExecuteNext, FName name, const FString &statement) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool Scene(bool &autoExecuteNext, const TMap<FName, FString> &params);
  virtual bool Scene_Implementation(bool &autoExecuteNext, const TMap<FName, FString> &params) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool Show(bool &autoExecuteNext, FRpyImage rpyImage, FName at, FName with);
  virtual bool Show_Implementation(bool &autoExecuteNext, FRpyImage rpyImage, FName at, FName with) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool Hide(bool &autoExecuteNext, FRpyImage rpyImage, FName at, FName with);
  virtual bool Hide_Implementation(bool &autoExecuteNext, FRpyImage rpyImage, FName at, FName with) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool PauseAudio(bool &autoExecuteNext, float timeout);
  virtual bool PauseAudio_Implementation(bool &autoExecuteNext, float timeout) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool StopAudio(bool &autoExecuteNext, FName channel, FRpyAudioOptions options);
  virtual bool StopAudio_Implementation(bool &autoExecuteNext, FName channel, FRpyAudioOptions options) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool QueueAudio(bool &autoExecuteNext, FName channel, FRpyAudio audio, FRpyAudioOptions options);
  virtual bool QueueAudio_Implementation(bool &autoExecuteNext, FName channel, FRpyAudio audio, FRpyAudioOptions options) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool PlayAudio(bool &autoExecuteNext, FName channel, FRpyAudio audio, FRpyAudioOptions options);
  virtual bool PlayAudio_Implementation(bool &autoExecuteNext, FName channel, FRpyAudio audio, FRpyAudioOptions options) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool Menu(bool &autoExecuteNext, const FString &statement, const TArray<FString> &choices);
  virtual bool Menu_Implementation(bool &autoExecuteNext, const FString &statement, const TArray<FString> &choices) = 0;

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
