#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Interfaces/RpyScriptInterpreterBaseManager.h"

#include "RpySceneManager.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API URpySceneManager : public URpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpySceneManager : public IRpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
public:
  // get state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  FRpySceneManagerState GetSceneManagerState() const;
  virtual FRpySceneManagerState GetSceneManagerState_Implementation() const = 0;

  // set state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void SetSceneManagerState(const FRpySceneManagerState &State);
  virtual void SetSceneManagerState_Implementation(const FRpySceneManagerState &State) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool Scene(FRpyImage rpyImage, FRpySceneOptions options);
  virtual bool Scene_Implementation(FRpyImage rpyImage, FRpySceneOptions options) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  FRpySceneManagerState GetCurrentSceneState();
  virtual FRpySceneManagerState GetCurrentSceneState_Implementation() const = 0;
};

// UCLASS()
// class KISHIVISUALNOVEL_API URpySceneManagerLibrary : public UBlueprintFunctionLibrary
// {
//   GENERATED_BODY()
// public:
// protected:
// private:

// };
