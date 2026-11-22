#include "Widgets/KishiSaveMenuWidgetBase.h"

#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "Interfaces/RpySaveSlot.h"
#include "Rpy/BaseRpyGameInstance.h"
#include "KishiRpyWidgetReflection.h"

void UKishiSaveMenuWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!IsDesignTime())
		WarnOnInterfaceEventOverrides(this);
	if (CancelButton)
		CancelButton->OnClicked.AddUniqueDynamic(this, &UKishiSaveMenuWidgetBase::HandleCancelClicked);
	if (UBaseRpyGameInstance *GameInstance = GetRpyGameInstance())
		GameInstance->OnSaveGameComplete.AddUniqueDynamic(this, &UKishiSaveMenuWidgetBase::HandleSaveGameComplete);
}

void UKishiSaveMenuWidgetBase::NativeDestruct()
{
	if (UBaseRpyGameInstance *GameInstance = GetRpyGameInstance())
		GameInstance->OnSaveGameComplete.RemoveDynamic(this, &UKishiSaveMenuWidgetBase::HandleSaveGameComplete);
	Super::NativeDestruct();
}

UBaseRpyGameInstance *UKishiSaveMenuWidgetBase::GetRpyGameInstance() const
{
	return IsDesignTime() ? nullptr : UBaseRpyGameInstance::GetBaseRpyGameInstance(const_cast<UKishiSaveMenuWidgetBase *>(this));
}

void UKishiSaveMenuWidgetBase::HandleOpen_Implementation(ERpySaveMenuMode InMode)
{
	Mode = InMode;
	bOpen = true;
	SetVisibility(ESlateVisibility::Visible);
	RefreshSlots();
	OnMenuOpened(Mode);
}

void UKishiSaveMenuWidgetBase::HandleClose_Implementation()
{
	SetVisibility(ESlateVisibility::Collapsed);
	// The interpreter and the menu can both close it; notify once.
	if (!bOpen)
		return;
	bOpen = false;
	OnMenuClosed();
}

void UKishiSaveMenuWidgetBase::RequestClose()
{
	UObject *InterpreterObject = Interpreter.GetObject();
	if (IsValid(InterpreterObject) && InterpreterObject->GetClass()->ImplementsInterface(URpyScriptInterpreter::StaticClass()))
	{
		if (Mode == ERpySaveMenuMode::Save)
			IRpyScriptInterpreter::Execute_setSaveMenuVisible(InterpreterObject, false);
		else
			IRpyScriptInterpreter::Execute_setLoadMenuVisible(InterpreterObject, false);
	}
	// Also close directly, in case there is no interpreter or it does not close its menu.
	IRpySaveMenu::Execute_HandleClose(this);
}

void UKishiSaveMenuWidgetBase::HandleCancelClicked()
{
	RequestClose();
}

void UKishiSaveMenuWidgetBase::HandleSaveGameComplete()
{
	if (bOpen)
		RefreshSlots();
}

void UKishiSaveMenuWidgetBase::RefreshSlots()
{
	SaveGames.Reset();
	UBaseRpyGameInstance *GameInstance = GetRpyGameInstance();
	if (!GameInstance)
	{
		if (!IsDesignTime())
			UE_LOG(LogTemp, Error, TEXT("%s: cannot list save slots, the Game Instance Class must derive from BaseRpyGameInstance."), *GetName());
	}
	else
	{
		if (bShowQuickSaveSlot)
		{
			UBaseRpySaveGame *QuickSave = GameInstance->GetQuickSaveGame(UserIndex);
			if (!QuickSave && Mode == ERpySaveMenuMode::Save)
			{
				QuickSave = NewObject<UBaseRpySaveGame>(this);
				QuickSave->isEmpty = true;
				QuickSave->SlotIndex = INDEX_NONE;
			}
			if (QuickSave)
			{
				// Loaded save games do not always keep the flag, and the slot needs it to choose quick save or quick load.
				QuickSave->isQuickSave = true;
				SaveGames.Add(QuickSave);
			}
		}
		SaveGames.Append(GameInstance->ListSaveSlots(UserIndex));
	}

	if (SlotsPanel)
	{
		SlotsPanel->ClearChildren();
		if (!SlotClass || !SlotClass->ImplementsInterface(URpySaveSlot::StaticClass()))
		{
			UE_LOG(LogTemp, Error, TEXT("%s: Slot Class must be set to a widget that implements IRpySaveSlot to list %d save slots."), *GetName(), SaveGames.Num());
		}
		else
		{
			TScriptInterface<IRpySaveMenu> Self;
			Self.SetObject(this);
			Self.SetInterface(Cast<IRpySaveMenu>(this));
			for (UBaseRpySaveGame *SaveGame : SaveGames)
			{
				UUserWidget *SlotWidget = CreateWidget<UUserWidget>(this, SlotClass);
				if (!SlotWidget)
					continue;
				IRpySaveSlot::Execute_SetSaveMenu(SlotWidget, Self);
				IRpySaveSlot::Execute_SetSaveGame(SlotWidget, SaveGame);
				SlotsPanel->AddChild(SlotWidget);
			}
		}
	}
	OnSlotsListed(SaveGames);
}

void UKishiSaveMenuWidgetBase::HandleSlotSelected_Implementation(UBaseRpySaveGame *SaveGame)
{
	UBaseRpyGameInstance *GameInstance = GetRpyGameInstance();
	if (!IsValid(SaveGame) || !GameInstance)
		return;

	if (Mode == ERpySaveMenuMode::Save)
	{
		const bool bSaved = SaveGame->isQuickSave ? GameInstance->QuickSave(UserIndex) : GameInstance->Save(SaveGame->SlotIndex, UserIndex);
		OnSlotUsed(SaveGame, Mode, bSaved);
		// The slots are listed again when the save finishes writing (OnSaveGameComplete).
		if (bSaved && bCloseAfterSave)
			RequestClose();
		return;
	}

	if (SaveGame->isEmpty)
		return;
	OnSlotUsed(SaveGame, Mode, true);
	// Close first: loading travels to the saved level.
	RequestClose();
	if (SaveGame->isQuickSave)
		GameInstance->QuickLoad(UserIndex);
	else
		GameInstance->Load(SaveGame->SlotIndex, UserIndex);
}
