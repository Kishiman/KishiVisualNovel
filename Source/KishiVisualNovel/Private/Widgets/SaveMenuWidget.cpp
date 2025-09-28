#include "Widgets/SaveMenuWidget.h"

#include "Components/PanelWidget.h"
#include "Kismet/GameplayStatics.h"

void USaveMenuWidget::RefreshSlots()
{
  UPanelWidget *SlotsPanel = GetSlotsPanel();
  TSubclassOf<USaveSlotWidget> SlotWidgetClass = GetSlotWidgetClass();
  if (!SlotsPanel || !SlotWidgetClass)
  {
    UE_LOG(LogTemp, Warning, TEXT("SaveMenuWidget: SlotsPanel or SlotWidgetClass is not set."));
    return;
  }

  SlotsPanel->ClearChildren();

  UBaseRpyGameInstance *GI = Cast<UBaseRpyGameInstance>(GetGameInstance());
  if (!GI)
  {
    UE_LOG(LogTemp, Warning, TEXT("SaveMenuWidget: GameInstance is not valid."));
    return;
  }

  TArray<UBaseRpySaveGame *> Saves = GI->ListSaveSlots();

  auto QuickSave = GI->GetQuickSaveGame();
  Saves.Insert(QuickSave, 0); // Quick Save at the top

  for (UBaseRpySaveGame *Save : Saves)
  {
    USaveSlotWidget *SaveSlot = CreateWidget<USaveSlotWidget>(this, SlotWidgetClass);
    if (SaveSlot)
    {
      SaveSlot->SetSaveGame(Save);
      SlotsPanel->AddChild(SaveSlot);
    }
    else
    {
      UE_LOG(LogTemp, Warning, TEXT("SaveMenuWidget: Failed to create SaveSlotWidget."));
    }
  }
}
