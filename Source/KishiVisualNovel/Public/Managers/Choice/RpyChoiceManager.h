#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Player/RpyScriptInterpreterBaseManager.h"
#include "Script/RpyScriptTypes.h"

#include "RpyChoiceManager.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API URpyChoiceManager : public URpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyChoiceManager : public IRpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
public:
  // get state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyChoiceManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  FRpyChoiceManagerState GetChoiceManagerState() const;
  virtual FRpyChoiceManagerState GetChoiceManagerState_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyChoiceManager::GetChoiceManagerState")); return {}; }

  // set state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyChoiceManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  void SetChoiceManagerState(const FRpyChoiceManagerState &State);
  virtual void SetChoiceManagerState_Implementation(const FRpyChoiceManagerState &State) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyChoiceManager::SetChoiceManagerState")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyChoiceManager|Required", meta = (ToolTip = "Required: display the choices; the player's pick goes to Choose. Return Wait, or Failed, after logging why, if they cannot be displayed. A menu always waits for a choice, so Continue is treated as Wait."))
  ERpyInstructionResult Menu(const TArray<FString> &choices);
  virtual ERpyInstructionResult Menu_Implementation(const TArray<FString> &choices) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyChoiceManager::Menu")); return ERpyInstructionResult::Failed; }

  // Handlers Interface

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyChoiceManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  void Choose(int idx);
  virtual void Choose_Implementation(int idx) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyChoiceManager::Choose")); }
};

// UCLASS()
// class KISHIVISUALNOVEL_API URpyChoiceManagerLibrary : public UBlueprintFunctionLibrary
// {
//   GENERATED_BODY()
// public:
// protected:
// private:
// };
