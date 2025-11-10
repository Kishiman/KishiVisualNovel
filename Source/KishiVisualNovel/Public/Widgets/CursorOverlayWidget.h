#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CursorOverlayWidget.generated.h"

UCLASS(Blueprintable)
class KISHIVISUALNOVEL_API UCursorOverlayWidget : public UUserWidget
{
  GENERATED_BODY()

public:
  virtual void NativeTick(const FGeometry &MyGeometry, float InDeltaTime) override;

  /** Centers the cursor widget on the screen */
  UFUNCTION(BlueprintCallable, Category = "Cursor")
  void CenterOnScreen();

  /** Sets the visual representation of the cursor */
  UFUNCTION(BlueprintCallable, Category = "Cursor")
  void SetCursorVisual(UWidget *NewCursorVisual);

protected:
  /** The root widget that visually represents the cursor (e.g., Image widget) */
  UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
  class UWidget *CursorVisual;
};
