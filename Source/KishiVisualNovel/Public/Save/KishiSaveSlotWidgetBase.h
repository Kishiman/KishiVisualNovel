#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Save/RpySaveSlot.h"
#include "KishiSaveSlotWidgetBase.generated.h"

class UButton;
class UImage;
class UTextBlock;
class UTexture2D;

/**
 * Native implementation of IRpySaveSlot. Create a child Widget Blueprint from this class and add a button named SlotButton.
 * Optionally add text blocks named LevelNameText and DateText and an image named ThumbnailImage; they are filled from the save game.
 */
UCLASS(Abstract, Blueprintable, meta = (DisplayName = "VN Save Slot Base"))
class KISHIVISUALNOVEL_API UKishiSaveSlotWidgetBase : public UUserWidget, public IRpySaveSlot
{
	GENERATED_BODY()

public:
	virtual void SetSaveMenu_Implementation(const TScriptInterface<IRpySaveMenu> &InSaveMenu) override { SaveMenu = InSaveMenu; }
	virtual void SetSaveGame_Implementation(UBaseRpySaveGame *InSaveGame) override;

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadOnly, Category = "Kishi Save Slot Widget Base")
	UBaseRpySaveGame *SaveGame = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Kishi Save Slot Widget Base")
	TScriptInterface<IRpySaveMenu> SaveMenu;

	/** Shown in LevelNameText for a slot that has never been saved. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kishi Save Slot Widget Base")
	FText EmptySlotText = NSLOCTEXT("KishiSaveSlot", "EmptySlot", "Empty");

	/** Put before the level name of the quick save. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kishi Save Slot Widget Base")
	FText QuickSaveText = NSLOCTEXT("KishiSaveSlot", "QuickSave", "Quick Save");

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Kishi Save Slot Widget Base")
	UButton *SlotButton = nullptr;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Kishi Save Slot Widget Base")
	UTextBlock *LevelNameText = nullptr;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Kishi Save Slot Widget Base")
	UTextBlock *DateText = nullptr;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Kishi Save Slot Widget Base")
	UImage *ThumbnailImage = nullptr;

	UPROPERTY(Transient)
	UTexture2D *ThumbnailTexture = nullptr;

	/** Optional hook: called after the slot shows a save game, to style empty or quick save slots. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi Save Slot Widget Base|Hooks")
	void OnSaveGameSet(UBaseRpySaveGame *InSaveGame);

	/** Selects this slot in its menu. SlotButton calls it when clicked. */
	UFUNCTION(BlueprintCallable, Category = "Kishi Save Slot Widget Base")
	void SelectSlot();

private:
	UFUNCTION()
	void HandleSlotButtonClicked();
};
