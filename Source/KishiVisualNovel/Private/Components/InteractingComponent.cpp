#include "Components/InteractingComponent.h"

#include "Utils/ArrayUtils.h"
#include "Utils/RayCastUtils.h"
#include "Kismet/GameplayStatics.h"

UInteractingComponent::UInteractingComponent()
{
  PrimaryComponentTick.bCanEverTick = true;
}

void UInteractingComponent::BeginPlay()
{
  Super::BeginPlay();
}
void UInteractingComponent::SetFocusedInteractable(UInteractableComponent *NewFocusedInteractable)
{
  auto previousFocusedInteractable = this->FocusedInteractable;
  if (previousFocusedInteractable != NewFocusedInteractable)
  {
    UE_LOG(LogTemp, Log, TEXT("%s on %s: focus changed to %s on %s"), *GetName(), *GetNameSafe(GetOwner()), *GetNameSafe(NewFocusedInteractable), *GetNameSafe(NewFocusedInteractable ? NewFocusedInteractable->GetOwner() : nullptr));
    if (previousFocusedInteractable)
    {
      previousFocusedInteractable->SetIsFocused(false);
    }
    if (NewFocusedInteractable)
    {
      NewFocusedInteractable->SetIsFocused(true);
    }
    this->FocusedInteractable = NewFocusedInteractable;
  }
}

void UInteractingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  ScanInteractableActors();
}

void UInteractingComponent::ScanInteractableActors()
{
  if (InteractionFlags.Num() <= interactablePerFlag.Num())
  {
    // Skip the scan while all flags are occupied, but drop the focus so its prompt hides during the interaction.
    if (this->FocusedInteractable != nullptr)
      SetFocusedInteractable(nullptr);
    return;
  }
  FHitResult focusHit;
  auto focusedInteractable = (UInteractableComponent *)URaycastUtils::RaycastForComponentFromPlayer(
      this,
      FocusDistance,
      UInteractableComponent::StaticClass(),
      focusHit,
      ECC_Visibility,
      false,
      0.0f);

  if (focusedInteractable &&
      focusedInteractable->State == EInteractableState::Interactable &&
      InteractionFlags.Contains(focusedInteractable->InteractionFlag) &&
      !interactablePerFlag.Contains(focusedInteractable->InteractionFlag))
  {
    SetFocusedInteractable(focusedInteractable);
  }
  else if (this->FocusedInteractable != nullptr)
  {
    SetFocusedInteractable(nullptr);
  }

  if (!this->bScanNearbyInteractables)
  {
    return;
  }
  auto PreviousNearbyInteractables = this->NearbyInteractables;
  this->NearbyInteractables.Empty();

  TArray<UInteractableComponent *> nearbyInteractables;
  TArray<FHitResult> outHits;

  TArray<UActorComponent *> nearbyComponents = URaycastUtils::SphereTraceForComponentsFromPlayer(
      this,
      NearbyRadius,
      UInteractableComponent::StaticClass(),
      outHits,
      ECC_Pawn,
      false,
      0.0f);
  // cast array to correct type
  for (UActorComponent *component : nearbyComponents)
  {
    if (UInteractableComponent *interactableComponent = Cast<UInteractableComponent>(component))
    {
      nearbyInteractables.Add(interactableComponent);
    }
  }

  for (UInteractableComponent *NearbyComponent : nearbyInteractables)
  {
    if (NearbyComponent && NearbyComponent->State == EInteractableState::Interactable)
    {
      this->NearbyInteractables.Add(NearbyComponent);
    }
  }
  TArray<UInteractableComponent *> ExistingActorsInter;
  TArray<UInteractableComponent *> RemovedActorsInter;
  TArray<UInteractableComponent *> AddedActorsInter;

  UArrayUtils::CompareArraysTyped<UInteractableComponent *>(
      PreviousNearbyInteractables,
      this->NearbyInteractables,
      /*Existing=*/ExistingActorsInter,
      /*Removed=*/RemovedActorsInter,
      /*Added=*/AddedActorsInter);
  for (UInteractableComponent *&RemovedActor : RemovedActorsInter)
  {
    RemovedActor->SetIsNearby(false);
  }
  for (UInteractableComponent *&AddedActor : AddedActorsInter)
  {
    AddedActor->SetIsNearby(true);
  }
  return;
}
UInteractableComponent *UInteractingComponent::CallInteractOnInteractable(FName ActionName)
{
  UInteractableComponent *Target = this->FocusedInteractable;
  if (!Target || !Target->CanInteract(ActionName, this))
  {
    UE_LOG(LogTemp, Warning, TEXT("%s on %s: action '%s' ignored, focused interactable is %s on %s."), *GetName(), *GetNameSafe(GetOwner()), *ActionName.ToString(), *GetNameSafe(Target), *GetNameSafe(Target ? Target->GetOwner() : nullptr));
    return nullptr;
  }
  this->interactablePerFlag.Add(Target->InteractionFlag, Target);
  // Everything is set up before Interact, because an interaction can end during the call (a light switch, a refused action).
  Target->OnInteractionEnd.AddUniqueDynamic(this, &UInteractingComponent::HandleInteractionEnded);
  this->OnInteractionStart.Broadcast(ActionName, this, Target);
  Target->Interact(ActionName, this);
  return Target;
}
void UInteractingComponent::HandleInteractionEnded(UInteractingComponent *interactingComponent, UInteractableComponent *interactedComponent)
{
  if (interactedComponent)
  {
    this->interactablePerFlag.Remove(interactedComponent->InteractionFlag);
    interactedComponent->OnInteractionEnd.RemoveDynamic(this, &UInteractingComponent::HandleInteractionEnded);
  }
  this->OnInteractionEnd.Broadcast(interactingComponent, interactedComponent);
}
void UInteractingComponent::CallEndInteractionOnInteractable() const
{
  // Copy: ending an interaction removes it from the map.
  TArray<UInteractableComponent *> ActiveInteractables;
  this->interactablePerFlag.GenerateValueArray(ActiveInteractables);
  for (UInteractableComponent *ActiveInteractable : ActiveInteractables)
  {
    if (IsValid(ActiveInteractable))
    {
      ActiveInteractable->EndInteraction();
    }
  }
}