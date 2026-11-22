#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Save/RpySaveMenu.h"
#include "Save/BaseRpySaveGame.h"

#include "RpySaveSlot.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API URpySaveSlot : public UInterface
{
  GENERATED_BODY()
};

/** One entry of a save menu. It shows a save game and, when selected, calls its menu's HandleSlotSelected. */
class KISHIVISUALNOVEL_API IRpySaveSlot
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpySaveSlot|Required", meta = (ToolTip = "Required: store the menu that owns this slot. Selecting the slot calls the menu's HandleSlotSelected."))
  void SetSaveMenu(const TScriptInterface<IRpySaveMenu> &InSaveMenu);
  virtual void SetSaveMenu_Implementation(const TScriptInterface<IRpySaveMenu> &InSaveMenu) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpySaveSlot::SetSaveMenu")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpySaveSlot|Required", meta = (ToolTip = "Required: show this save game. It may be an empty placeholder (isEmpty) for a slot that has never been saved."))
  void SetSaveGame(UBaseRpySaveGame *SaveGame);
  virtual void SetSaveGame_Implementation(UBaseRpySaveGame *SaveGame) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpySaveSlot::SetSaveGame")); }
};
