#include "Utils/PlayerControllerUtils.h"

void UPlayerControllerUtils::SetPlayerInputMode(APlayerController *PC, EPlayerInputMode NewMode)
{
  if (!PC)
    return;
  switch (NewMode)
  {
  case EPlayerInputMode::Gameplay:
  {
    PC->SetIgnoreMoveInput(false);
    PC->SetIgnoreLookInput(false);
    PC->SetInputMode(FInputModeGameOnly());
    PC->bShowMouseCursor = false;
  }
  break;

  case EPlayerInputMode::GameAndUI:
  {
    PC->SetIgnoreMoveInput(true);
    PC->SetIgnoreLookInput(true);
    FInputModeGameAndUI Mode;
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    PC->SetInputMode(Mode);
    PC->bShowMouseCursor = true;
  }
  break;

  case EPlayerInputMode::UIOnly:
  {
    PC->SetIgnoreMoveInput(true);
    PC->SetIgnoreLookInput(true);
    PC->SetInputMode(FInputModeUIOnly());
    PC->bShowMouseCursor = true;
  }
  break;
  }
}

void UPlayerControllerUtils::RestoreDefaultInputMode(APlayerController *PC)
{
  SetPlayerInputMode(PC, EPlayerInputMode::Gameplay);
}
