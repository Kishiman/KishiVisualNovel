#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Interfaces/RpySavableActor.h"
#include "Rpy/RpySession.h"

#include "RpySaveGame.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API URpySaveGame : public UInterface
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpySaveGame
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  TArray<uint8> GetScreenshot() const;
  virtual TArray<uint8> GetScreenshot_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void SetScreenshot(const TArray<uint8> &Screenshot);
  virtual void SetScreenshot_Implementation(const TArray<uint8> &Screenshot) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  FString GetLevelName() const;
  virtual FString GetLevelName_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void SetLevelName(const FString &LevelName);
  virtual void SetLevelName_Implementation(const FString &LevelName) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  FRpyState GetRpyState() const;
  virtual FRpyState GetRpyState_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void SetRpyState(const FRpyState &State);
  virtual void SetRpyState_Implementation(const FRpyState &State) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  URpySavableActorState *GetSavableActorState(const AActor *Actor) const;
  virtual URpySavableActorState *GetSavableActorState_Implementation(const AActor *Actor) const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void SetSavableActorState(const AActor *Actor, const URpySavableActorState *State);
  virtual void SetSavableActorState_Implementation(const AActor *Actor, const URpySavableActorState *State) = 0;
};

UCLASS()
class KISHIVISUALNOVEL_API URpySaveGameLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
protected:
private:
};
