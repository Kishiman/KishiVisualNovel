#include "Save/KishiSaveSlotWidgetBase.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Utils/MediaUtils.h"
#include "Player/KishiRpyWidgetReflection.h"

void UKishiSaveSlotWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!IsDesignTime())
		WarnOnInterfaceEventOverrides(this);
	if (SlotButton)
		SlotButton->OnClicked.AddUniqueDynamic(this, &UKishiSaveSlotWidgetBase::HandleSlotButtonClicked);
	else if (!IsDesignTime())
		UE_LOG(LogTemp, Error, TEXT("%s: add a Button named SlotButton, or call Select Slot yourself, so the slot can be selected."), *GetNameSafe(GetClass()));
}

void UKishiSaveSlotWidgetBase::SetSaveGame_Implementation(UBaseRpySaveGame *InSaveGame)
{
	SaveGame = InSaveGame;
	const bool bEmpty = !IsValid(SaveGame) || SaveGame->isEmpty;

	if (LevelNameText)
	{
		FText Label = bEmpty ? EmptySlotText : FText::FromString(IRpySaveGame::Execute_GetLevelName(SaveGame));
		if (IsValid(SaveGame) && SaveGame->isQuickSave)
			Label = FText::Format(NSLOCTEXT("KishiSaveSlot", "QuickSaveLabel", "{0}: {1}"), QuickSaveText, Label);
		LevelNameText->SetText(Label);
	}
	if (DateText)
		DateText->SetText(bEmpty ? FText::GetEmpty() : FText::AsDateTime(SaveGame->SaveDate));

	ThumbnailTexture = nullptr;
	if (!bEmpty)
	{
		const TArray<uint8> Screenshot = IRpySaveGame::Execute_GetScreenshot(SaveGame);
		if (Screenshot.Num() > 0)
			ThumbnailTexture = UMediaUtils::CreateTextureFromPNG(Screenshot);
	}
	if (ThumbnailImage)
	{
		if (ThumbnailTexture)
			ThumbnailImage->SetBrushFromTexture(ThumbnailTexture);
		ThumbnailImage->SetVisibility(ThumbnailTexture ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
	}

	// An empty slot can be saved into but not loaded.
	UObject *MenuObject = SaveMenu.GetObject();
	if (SlotButton && IsValid(MenuObject) && MenuObject->GetClass()->ImplementsInterface(URpySaveMenu::StaticClass()))
		SlotButton->SetIsEnabled(!(bEmpty && IRpySaveMenu::Execute_GetSaveMenuMode(MenuObject) == ERpySaveMenuMode::Load));

	OnSaveGameSet(SaveGame);
}

void UKishiSaveSlotWidgetBase::HandleSlotButtonClicked()
{
	SelectSlot();
}

void UKishiSaveSlotWidgetBase::SelectSlot()
{
	UObject *MenuObject = SaveMenu.GetObject();
	if (!IsValid(MenuObject) || !MenuObject->GetClass()->ImplementsInterface(URpySaveMenu::StaticClass()))
	{
		UE_LOG(LogTemp, Error, TEXT("%s: selected without a save menu; the menu must call SetSaveMenu on its slots."), *GetName());
		return;
	}
	IRpySaveMenu::Execute_HandleSlotSelected(MenuObject, SaveGame);
}
