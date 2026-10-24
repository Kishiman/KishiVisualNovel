#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Interfaces/RpyScriptInterpreterBaseManager.h"

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

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyChoiceManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  bool Menu(const TArray<FString> &choices);
  virtual bool Menu_Implementation(const TArray<FString> &choices) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyChoiceManager::Menu")); return {}; }

  // Handlers Interface

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyChoiceManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  void Choose(int idx);
  virtual void Choose_Implementation(int idx) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyChoiceManager::Choose")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyChoiceManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  FRpyChoiceManagerState GetCurrentChoiceState();
  virtual FRpyChoiceManagerState GetCurrentChoiceState_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyChoiceManager::GetCurrentChoiceState")); return {}; }
};

// UCLASS()
// class KISHIVISUALNOVEL_API URpyChoiceManagerLibrary : public UBlueprintFunctionLibrary
// {
//   GENERATED_BODY()
// public:
// protected:
// private:
// };
