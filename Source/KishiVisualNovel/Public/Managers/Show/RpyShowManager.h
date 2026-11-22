#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Player/RpyScriptInterpreterBaseManager.h"
#include "Script/RpyScriptTypes.h"

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

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyShowManager|Required", meta = (ToolTip = "Required: show the image under its tag, or update it if that tag is already shown. Return Continue once it is shown (a transition may still be playing), Wait if you will report completion through the interpreter yourself, or Failed, after logging why, if it cannot be shown."))
  ERpyInstructionResult Show(FRpyImage rpyImage, FRpySceneOptions options);
  virtual ERpyInstructionResult Show_Implementation(FRpyImage rpyImage, FRpySceneOptions options) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyShowManager::Show")); return ERpyInstructionResult::Failed; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyShowManager|Required", meta = (ToolTip = "Required: hide the image with this tag. Return Continue when it is hidden or when no image has that tag, or Failed, after logging why, if it cannot be hidden."))
  ERpyInstructionResult Hide(FName tag, FRpySceneOptions options);
  virtual ERpyInstructionResult Hide_Implementation(FName tag, FRpySceneOptions options) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyShowManager::Hide")); return ERpyInstructionResult::Failed; }
};

// UCLASS()
// class KISHIVISUALNOVEL_API URpyShowManagerLibrary : public UBlueprintFunctionLibrary
// {
//   GENERATED_BODY()
// public:
// protected:
// private:
// };
