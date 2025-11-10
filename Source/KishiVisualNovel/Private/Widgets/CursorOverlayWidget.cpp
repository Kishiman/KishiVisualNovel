#include "Widgets/CursorOverlayWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void UCursorOverlayWidget::NativeTick(const FGeometry &MyGeometry, float InDeltaTime)
{
  Super::NativeTick(MyGeometry, InDeltaTime);

  APlayerController *PC = UGameplayStatics::GetPlayerController(this, 0);
  if (!PC)
    return;

  if (PC->bShowMouseCursor == false)
  {
    CenterOnScreen();
    return;
  }

  // Get mouse screen position
  float MouseX, MouseY;
  if (!PC->GetMousePosition(MouseX, MouseY))
  {
    return;
  }
  FVector2D CursorOffset(0.0f, 0.0f);
  if (CursorVisual)
  {
    const FVector2D Size = CursorVisual->GetDesiredSize();

    // Center offset = half size in negative direction
    CursorOffset = FVector2D(-Size.X * 0.25f, -Size.Y * 0.25f);
  }

  // Apply offset
  const FVector2D ScreenPosition(MouseX, MouseY);
  const FVector2D FinalPos = ScreenPosition + CursorOffset;

  // Set widget location
  // Requires Screen Space mode
  UE_LOG(LogTemp, Display, TEXT("Setting cursor overlay position from: %s to: %s"), *ScreenPosition.ToString(), *FinalPos.ToString());
  // ignore DPI scaling for now
  SetPositionInViewport(FinalPos, true);
  // Optional: If the cursor graphic should rotate or animate, do it here
}

void UCursorOverlayWidget::CenterOnScreen()
{
  if (!IsInViewport())
    return;

  // Get viewport size
  FVector2D ViewportSize;
  GEngine->GameViewport->GetViewportSize(ViewportSize);

  // Get widget size (based on final layout)
  const FVector2D WidgetSize = GetDesiredSize();

  // Compute centered position
  const FVector2D CenterPos(
      (ViewportSize.X - WidgetSize.X) * 0.5f,
      (ViewportSize.Y - WidgetSize.Y) * 0.5f);

  SetPositionInViewport(CenterPos, true);
}

void UCursorOverlayWidget::SetCursorVisual(UWidget *NewCursorVisual)
{
  CursorVisual = NewCursorVisual;
}