#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Widgets/SaveSlotWidget.h"
#include "Rpy/BaseRpyGameInstance.h"

#include "SaveMenuWidget.generated.h"

UCLASS(Abstract)
class KISHIVISUALNOVEL_API USaveMenuWidget : public UUserWidget
{
  GENERATED_BODY()

public:
  /** Refresh the list of save slots */
  UFUNCTION(BlueprintCallable, Category = "Save")
  virtual void RefreshSlots();

protected:
  UFUNCTION(BlueprintImplementableEvent, Category = "Save")
  TSubclassOf<USaveSlotWidget> GetSlotWidgetClass() const;

  UFUNCTION(BlueprintImplementableEvent, Category = "Save")
  UPanelWidget *GetSlotsPanel() const;
};
