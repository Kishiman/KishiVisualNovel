#include "Components/InteractableComponent.h"

#include "Components/InteractingComponent.h"

UInteractableComponent::UInteractableComponent()
{
  PrimaryComponentTick.bCanEverTick = false;
}

void UInteractableComponent::SetState(EInteractableState NewState)
{
  State = NewState;
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

void UInteractableComponent::EndInteraction() const
{
  OnInteractionEnd.Broadcast();
}

void UInteractableComponent::Interact(FName ActionName, UInteractingComponent *interactingComponent)
{
  if (State != EInteractableState::Interactable || ActionName == NAME_None || !interactingComponent)
    return;

  AActor *InstigatingActor = interactingComponent->GetOwner();

  State = EInteractableState::Interacting;
  OnInteractionStart.Broadcast(ActionName, InstigatingActor, GetOwner());
}
