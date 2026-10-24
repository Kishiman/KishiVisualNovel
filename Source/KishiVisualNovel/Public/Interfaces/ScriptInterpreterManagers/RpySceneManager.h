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
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpySceneManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  FRpySceneManagerState GetSceneManagerState() const;
  virtual FRpySceneManagerState GetSceneManagerState_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpySceneManager::GetSceneManagerState")); return {}; }

  // set state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpySceneManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  void SetSceneManagerState(const FRpySceneManagerState &State);
  virtual void SetSceneManagerState_Implementation(const FRpySceneManagerState &State) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpySceneManager::SetSceneManagerState")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpySceneManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  bool Scene(FRpyImage rpyImage, FRpySceneOptions options);
  virtual bool Scene_Implementation(FRpyImage rpyImage, FRpySceneOptions options) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpySceneManager::Scene")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpySceneManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  FRpySceneManagerState GetCurrentSceneState();
  virtual FRpySceneManagerState GetCurrentSceneState_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpySceneManager::GetCurrentSceneState")); return {}; }
};

// UCLASS()
// class KISHIVISUALNOVEL_API URpySceneManagerLibrary : public UBlueprintFunctionLibrary
// {
//   GENERATED_BODY()
// public:
// protected:
// private:

// };
