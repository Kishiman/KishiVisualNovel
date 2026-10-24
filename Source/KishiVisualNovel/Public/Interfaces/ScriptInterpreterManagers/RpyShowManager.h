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
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyShowManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  FRpyShowManagerState GetShowManagerState() const;
  virtual FRpyShowManagerState GetShowManagerState_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyShowManager::GetShowManagerState")); return {}; }

  // set state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyShowManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  void SetShowManagerState(const FRpyShowManagerState &State);
  virtual void SetShowManagerState_Implementation(const FRpyShowManagerState &State) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyShowManager::SetShowManagerState")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyShowManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  bool Show(FRpyImage rpyImage, FRpySceneOptions options);
  virtual bool Show_Implementation(FRpyImage rpyImage, FRpySceneOptions options) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyShowManager::Show")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyShowManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  bool Hide(FName tag, FRpySceneOptions options);
  virtual bool Hide_Implementation(FName tag, FRpySceneOptions options) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyShowManager::Hide")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyShowManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  TArray<FRpyShowManagerState> GetCurrentShowStates();
  virtual TArray<FRpyShowManagerState> GetCurrentShowStates_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyShowManager::GetCurrentShowStates")); return {}; }
};

// UCLASS()
// class KISHIVISUALNOVEL_API URpyShowManagerLibrary : public UBlueprintFunctionLibrary
// {
//   GENERATED_BODY()
// public:
// protected:
// private:
// };
