#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Player/RpyScriptInterpreter.h"
#include "Save/BaseRpySaveGame.h"

#include "RpySaveMenu.generated.h"

/** What selecting a slot does in a save menu. */
UENUM(BlueprintType)
enum class ERpySaveMenuMode : uint8
{
  Save,
  Load
};

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API URpySaveMenu : public UInterface
{
  GENERATED_BODY()
};

/**
 * A menu that lists the save slots and saves into or loads from the selected one.
 * The VN player finds it in its widget tree, gives it the interpreter, and opens it from setSaveMenuVisible and setLoadMenuVisible.
 */
class KISHIVISUALNOVEL_API IRpySaveMenu
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpySaveMenu|Required", meta = (ToolTip = "Required: store the interpreter that opens this menu. The menu closes through it, so the interpreter's menu state stays in sync. It may be None when the menu is used without a VN player."))
  void SetInterpreter(const TScriptInterface<IRpyScriptInterpreter> &InInterpreter);
  virtual void SetInterpreter_Implementation(const TScriptInterface<IRpyScriptInterpreter> &InInterpreter) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpySaveMenu::SetInterpreter")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpySaveMenu|Required", meta = (ToolTip = "Required: list the save slots and show the menu in the given mode."))
  void HandleOpen(ERpySaveMenuMode Mode);
  virtual void HandleOpen_Implementation(ERpySaveMenuMode Mode) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpySaveMenu::HandleOpen")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpySaveMenu|Required", meta = (ToolTip = "Required: hide the menu. Called by the interpreter; to close the menu from inside it, ask the interpreter to hide its save or load menu instead."))
  void HandleClose();
  virtual void HandleClose_Implementation() { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpySaveMenu::HandleClose")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpySaveMenu|Required", meta = (ToolTip = "Required: save into or load from the selected slot, according to the menu's mode. Save slots call this when they are clicked."))
  void HandleSlotSelected(UBaseRpySaveGame *SaveGame);
  virtual void HandleSlotSelected_Implementation(UBaseRpySaveGame *SaveGame) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpySaveMenu::HandleSlotSelected")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpySaveMenu|Required", meta = (ToolTip = "Required: return the mode the menu was last opened in."))
  ERpySaveMenuMode GetSaveMenuMode() const;
  virtual ERpySaveMenuMode GetSaveMenuMode_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpySaveMenu::GetSaveMenuMode")); return ERpySaveMenuMode::Save; }
};
