#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Interfaces/RpyScriptInterpreterBaseManager.h"

#include "RpyShowManager.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API URpyShowManager : public URpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyShowManager : public IRpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
public:
  // get state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  FRpyShowManagerState GetShowManagerState() const;
  virtual FRpyShowManagerState GetShowManagerState_Implementation() const = 0;

  // set state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void SetShowManagerState(const FRpyShowManagerState &State);
  virtual void SetShowManagerState_Implementation(const FRpyShowManagerState &State) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool Show(FRpyImage rpyImage, FRpySceneOptions options);
  virtual bool Show_Implementation(FRpyImage rpyImage, FRpySceneOptions options) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool Hide(FName tag, FRpySceneOptions options);
  virtual bool Hide_Implementation(FName tag, FRpySceneOptions options) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  TArray<FRpyShowManagerState> GetCurrentShowStates();
  virtual TArray<FRpyShowManagerState> GetCurrentShowStates_Implementation() const = 0;
};

// UCLASS()
// class KISHIVISUALNOVEL_API URpyShowManagerLibrary : public UBlueprintFunctionLibrary
// {
//   GENERATED_BODY()
// public:
// protected:
// private:
// };
