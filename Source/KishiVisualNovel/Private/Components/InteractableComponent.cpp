#include "Components/InteractableComponent.h"

#include "Components/InteractingComponent.h"

UInteractableComponent::UInteractableComponent()
{
  PrimaryComponentTick.bCanEverTick = false;
}

AActor *UInteractableComponent::GetInteractingActor() const
{
  return CurrentInteractingComponent ? CurrentInteractingComponent->GetOwner() : nullptr;
}

void UInteractableComponent::SetIsFocused(bool bFocused)
{
  bIsFocused = bFocused;
  OnFocusChanged.Broadcast(bFocused);
}

void UInteractableComponent::SetIsNearby(bool bNearby)
{
  bIsNearby = bNearby;
  OnNearbyChanged.Broadcast(bNearby);
}

void UInteractableComponent::EndInteraction()
{
  auto interactingComponent = this->CurrentInteractingComponent;
  this->CurrentInteractingComponent = nullptr;
  OnInteractionEnd.Broadcast(interactingComponent, this);
}

void UInteractableComponent::Interact(FName ActionName, UInteractingComponent *InteractingComponent)
{
  if (State != EInteractableState::Interactable || ActionName == NAME_None || !InteractingComponent)
    return;
  this->CurrentInteractingComponent = InteractingComponent;

  State = EInteractableState::Interacting;
  OnInteractionStart.Broadcast(ActionName, InteractingComponent, this);
}
