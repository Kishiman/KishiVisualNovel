#include "Widgets/SaveSlotWidget.h"

#include "ImageUtils.h"

void USaveSlotWidget::SetSaveGame(UBaseRpySaveGame *Save)
{
  CurrentSaveGame = Save;
  if (!Save || !Save->IsValidLowLevel() || Save->LevelName.IsEmpty())
  {
    UE_LOG(LogTemp, Warning, TEXT("SaveSlotWidget: No valid save assigned"));
    OnSaveGameEmpty(); // Let Blueprint handle UI
    return;
  }
  OnSaveGameAssigned(); // Let Blueprint handle UI
}