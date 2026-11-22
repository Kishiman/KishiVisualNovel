#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Player/RpyScriptInterpreterBaseManager.h"
#include "Script/RpyScriptTypes.h"
#include "Utils/StringUtils.h"

#include "RpyStatementManager.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API URpyStatementManager : public URpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyStatementManager : public IRpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
public:
  // get state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyStatementManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  FRpyStatementManagerState GetStatementManagerState() const;
  virtual FRpyStatementManagerState GetStatementManagerState_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyStatementManager::GetStatementManagerState")); return {}; }

  // set state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyStatementManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  void SetStatementManagerState(const FRpyStatementManagerState &State);
  virtual void SetStatementManagerState_Implementation(const FRpyStatementManagerState &State) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyStatementManager::SetStatementManagerState")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyStatementManager|Required", meta = (ToolTip = "Required: display the line. Return Wait and report completion through the interpreter when the player has read it, Continue only for a line that should not wait, or Failed, after logging why, if it cannot be displayed."))
  ERpyInstructionResult Say(FName name, const FString &statement);
  virtual ERpyInstructionResult Say_Implementation(FName name, const FString &statement) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyStatementManager::Say")); return ERpyInstructionResult::Failed; }
};

/** Shared logic for IRpyStatementManager implementers: the widget Base calls these, and a component or Blueprint manager can call them the same way. */
UCLASS()
class KISHIVISUALNOVEL_API URpyStatementManagerLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintPure, Category = "Rpy Statement Manager Library", meta = (ToolTip = "Return the name to display for a speaker. Narration (None) displays nothing instead of 'None'."))
  static FString GetSpeakerDisplayName(FName Name);

  UFUNCTION(BlueprintCallable, Category = "Rpy Statement Manager Library", meta = (DefaultToSelf = "Target", ToolTip = "Open the interpreter's save menu, or close it if it is open. Returns false, after logging why, if the manager has no interpreter."))
  static bool ToggleSaveMenu(const TScriptInterface<IRpyStatementManager> &Target);

  UFUNCTION(BlueprintCallable, Category = "Rpy Statement Manager Library", meta = (DefaultToSelf = "Target", ToolTip = "Open the interpreter's load menu, or close it if it is open. Returns false, after logging why, if the manager has no interpreter."))
  static bool ToggleLoadMenu(const TScriptInterface<IRpyStatementManager> &Target);

  UFUNCTION(BlueprintCallable, Category = "Rpy Statement Manager Library", meta = (WorldContext = "WorldContextObject", ToolTip = "Quick save through the game instance, which must derive from BaseRpyGameInstance. Returns false, after logging why, if it does not, or the save fails."))
  static bool QuickSave(UObject *WorldContextObject);

  UFUNCTION(BlueprintCallable, Category = "Rpy Statement Manager Library", meta = (WorldContext = "WorldContextObject", ToolTip = "Quick load through the game instance, which must derive from BaseRpyGameInstance. Returns false, after logging why, if it does not."))
  static bool QuickLoad(UObject *WorldContextObject);

  /**
   * Send the interpreter the viseme for the characters revealed between PreviousCount and Count of Statement. LastViseme holds the last spoken viseme:
   * the mouth keeps that shape through Neutral characters, and the time offset is the time since it at TextSpeed characters per second, or a long silence
   * once the line is fully revealed.
   */
  static void EmitDialogueViseme(const TScriptInterface<IRpyStatementManager> &Target, FName Speaker, const FTaggedString &Statement, int32 PreviousCount, int32 Count,
                                 float TextSpeed, FMouthViseme &LastViseme);
};
