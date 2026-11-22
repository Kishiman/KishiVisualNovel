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

/** Shared logic for IRpyChoiceManager implementers: the widget Base calls these, and a component or Blueprint manager can call them the same way. */
UCLASS()
class KISHIVISUALNOVEL_API URpyChoiceManagerLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable, Category = "Rpy Choice Manager Library", meta = (DefaultToSelf = "Target", ToolTip = "Default SetChoiceManagerState: call Menu on the manager with the saved choices."))
  static void SetChoiceManagerState_Default(const TScriptInterface<IRpyChoiceManager> &Target, const FRpyChoiceManagerState &State);

  UFUNCTION(BlueprintCallable, Category = "Rpy Choice Manager Library", meta = (DefaultToSelf = "Target", ToolTip = "Default Choose: send the picked index to the session, which jumps to the choice's block. Clear the displayed choices first, since the session may reach the next menu before this returns. Returns false, after logging why, if there is no session; the session logs an index that is not a choice."))
  static bool Choose_Default(const TScriptInterface<IRpyChoiceManager> &Target, int32 Index);
};
