#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "Rpy/BaseRpySaveGame.h"

#include "SaveSlotWidget.generated.h"

UCLASS(Abstract) // Abstract, meant to be extended in BP
class KISHIVISUALNOVEL_API USaveSlotWidget : public UUserWidget
{
  GENERATED_BODY()

public:
  /** Called by menu to update slot */
  UFUNCTION(BlueprintCallable, Category = "Save")
  void SetSaveGame(UBaseRpySaveGame *Save);

protected:
  UPROPERTY(BlueprintReadOnly, Category = "Save")
  UBaseRpySaveGame *CurrentSaveGame;

  /** Override in BP for visuals */
  UFUNCTION(BlueprintImplementableEvent, Category = "Save")
  void OnSaveGameAssigned();

  UFUNCTION(BlueprintImplementableEvent, Category = "Save")
  void OnSaveGameEmpty();
};
