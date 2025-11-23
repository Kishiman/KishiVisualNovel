#include "Components/CameraAnchorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/InteractingComponent.h"

UCameraAnchorComponent::UCameraAnchorComponent()
{
  PrimaryComponentTick.bCanEverTick = true;
  bIsAnchored = false;
}

void UCameraAnchorComponent::BeginPlay()
{
  Super::BeginPlay();

  // Find ANY camera component sibling
  AActor *Owner = GetOwner();
  if (Owner)
  {
    ControlledCamera = Owner->FindComponentByClass<UCameraComponent>();
  }
}

void UCameraAnchorComponent::Activate(bool bReset)
{
  Super::Activate(bReset);

  if (!ControlledCamera)
    return;

  APlayerController *PC = UGameplayStatics::GetPlayerController(this, 0);
  if (!PC)
    return;
  SetComponentTickEnabled(true);
  int32 SizeX, SizeY;
  PC->GetViewportSize(SizeX, SizeY);
  PC->SetMouseLocation(SizeX / 2, SizeY / 2);

  // Enable cursor for camera rotation logic
  PC->bShowMouseCursor = true;
  PC->SetInputMode(FInputModeGameAndUI());

  // Store initial rotation
  InitialRotation = ControlledCamera->GetRelativeRotation();

  bIsAnchored = true;
}

void UCameraAnchorComponent::Deactivate()
{
  if (bIsAnchored)
  {
    APlayerController *PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC)
    {
      PC->bShowMouseCursor = false;
      PC->SetInputMode(FInputModeGameOnly());
    }
  }

  SetComponentTickEnabled(false);
  bIsAnchored = false;

  Super::Deactivate();
}

void UCameraAnchorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  if (!bIsAnchored || !ControlledCamera)
    return;

  APlayerController *PC = UGameplayStatics::GetPlayerController(this, 0);
  if (!PC)
    return;

  int32 ViewX, ViewY;
  PC->GetViewportSize(ViewX, ViewY);

  float MouseX, MouseY;
  PC->GetMousePosition(MouseX, MouseY);

  FRotator Rot = ControlledCamera->GetRelativeRotation();

  // Clamp inside threshold
  const float MinX = edgeMarginPxl;
  const float MaxX = ViewX - edgeMarginPxl;
  const float MinY = edgeMarginPxl;
  const float MaxY = ViewY - edgeMarginPxl;
  // Turn yaw
  if (MouseX < MinX + edgeBorderPxl)
  {
    Rot.Yaw -= TurnSpeed * DeltaTime;
  }
  else if (MouseX > MaxX - edgeBorderPxl)
  {
    Rot.Yaw += TurnSpeed * DeltaTime;
  }

  // Turn pitch (Y is down in screen coords)
  if (MouseY < MinY + edgeBorderPxl)
  {
    Rot.Pitch += TurnSpeed * DeltaTime;
  }
  else if (MouseY > MaxY - edgeBorderPxl)
  {
    Rot.Pitch -= TurnSpeed * DeltaTime;
  }

  // Clamp
  Rot.Yaw = FMath::Clamp(Rot.Yaw, InitialRotation.Yaw + MinYaw, InitialRotation.Yaw + MaxYaw);
  Rot.Pitch = FMath::Clamp(Rot.Pitch, InitialRotation.Pitch + MinPitch, InitialRotation.Pitch + MaxPitch);

  ControlledCamera->SetRelativeRotation(Rot);

  float ClampedX = FMath::Clamp(MouseX, MinX, MaxX);
  float ClampedY = FMath::Clamp(MouseY, MinY, MaxY);

  if (ClampedX != MouseX || ClampedY != MouseY)
  {
    PC->SetMouseLocation(ClampedX, ClampedY);
  }
}
