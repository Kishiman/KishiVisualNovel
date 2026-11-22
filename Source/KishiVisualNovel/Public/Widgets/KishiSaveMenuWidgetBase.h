#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/RpySaveMenu.h"
#include "KishiSaveMenuWidgetBase.generated.h"

class UButton;
class UPanelWidget;
class UBaseRpyGameInstance;

/**
 * Native implementation of IRpySaveMenu. Create a child Widget Blueprint from this class, add a panel named
 * SlotsPanel and, optionally, a button named CancelButton, and set Slot Class to a widget that implements IRpySaveSlot.
 * Saving and loading go through the game instance, which must derive from BaseRpyGameInstance.
 */
UCLASS(Abstract, Blueprintable, meta = (DisplayName = "VN Save Menu Base"))
class KISHIVISUALNOVEL_API UKishiSaveMenuWidgetBase : public UUserWidget, public IRpySaveMenu
{
	GENERATED_BODY()

public:
	virtual void SetInterpreter_Implementation(const TScriptInterface<IRpyScriptInterpreter> &InInterpreter) override { Interpreter = InInterpreter; }
	virtual void HandleOpen_Implementation(ERpySaveMenuMode InMode) override;
	virtual void HandleClose_Implementation() override;
	virtual void HandleSlotSelected_Implementation(UBaseRpySaveGame *SaveGame) override;
	virtual ERpySaveMenuMode GetSaveMenuMode_Implementation() const override { return Mode; }

	/** Reads the slots from disk again and rebuilds SlotsPanel. */
	UFUNCTION(BlueprintCallable, Category = "Kishi Save Menu Widget Base")
	void RefreshSlots();

	/** Closes the menu through the interpreter when there is one, so its save and load menu state stays in sync. */
	UFUNCTION(BlueprintCallable, Category = "Kishi Save Menu Widget Base")
	void RequestClose();

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "Kishi Save Menu Widget Base")
	ERpySaveMenuMode Mode = ERpySaveMenuMode::Save;

	UPROPERTY(BlueprintReadOnly, Category = "Kishi Save Menu Widget Base")
	TScriptInterface<IRpyScriptInterpreter> Interpreter;

	/** Widget created for each slot. It must implement IRpySaveSlot. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kishi Save Menu Widget Base", meta = (MustImplement = "RpySaveSlot"))
	TSubclassOf<UUserWidget> SlotClass;

	/** Lists the quick save first: when it exists, and in Save mode also as an empty slot to quick save into. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kishi Save Menu Widget Base")
	bool bShowQuickSaveSlot = true;

	/** Closes the menu after saving into a slot. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kishi Save Menu Widget Base")
	bool bCloseAfterSave = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kishi Save Menu Widget Base")
	int32 UserIndex = 0;

	/** Save games listed by the last RefreshSlots, quick save first. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Kishi Save Menu Widget Base")
	TArray<UBaseRpySaveGame *> SaveGames;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Kishi Save Menu Widget Base")
	UPanelWidget *SlotsPanel = nullptr;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Kishi Save Menu Widget Base")
	UButton *CancelButton = nullptr;

	/** Optional hook: called after the menu opens and lists its slots. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi Save Menu Widget Base|Hooks")
	void OnMenuOpened(ERpySaveMenuMode OpenedMode);

	/** Optional hook: called after the menu closes. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi Save Menu Widget Base|Hooks")
	void OnMenuClosed();

	/** Optional hook: called after the slots are listed, for example to build the slots yourself when there is no SlotsPanel. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi Save Menu Widget Base|Hooks")
	void OnSlotsListed(const TArray<UBaseRpySaveGame *> &ListedSaveGames);

	/** Optional hook: called after a slot is saved into or loading from it starts. Saving finishes in the background. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi Save Menu Widget Base|Hooks")
	void OnSlotUsed(UBaseRpySaveGame *SaveGame, ERpySaveMenuMode UsedMode, bool bSucceeded);

private:
	UBaseRpyGameInstance *GetRpyGameInstance() const;
	UFUNCTION()
	void HandleCancelClicked();
	UFUNCTION()
	void HandleSaveGameComplete();
	bool bOpen = false;
};
