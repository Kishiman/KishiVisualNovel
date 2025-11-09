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
    // skip scan if all flags are occupied
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

  if (focusedInteractable)
  {
    UE_LOG(LogTemp, Warning, TEXT("Focused Interactable: %s"), *focusedInteractable->GetName());
  }

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
  if (this->FocusedInteractable)
  {
    this->interactablePerFlag.Add(this->FocusedInteractable->InteractionFlag, this->FocusedInteractable);
    this->FocusedInteractable->Interact(ActionName, this);
    this->OnInteractionStart.Broadcast(ActionName, this, this->FocusedInteractable);
    this->FocusedInteractable->OnInteractionEnd.AddDynamic(this, &UInteractingComponent::HandleInteractionEnded);
    return this->FocusedInteractable;
  }
  return nullptr;
}
void UInteractingComponent::HandleInteractionEnded(UInteractingComponent *interactingComponent, UInteractableComponent *interactedComponent)
{
  this->interactablePerFlag.Remove(this->FocusedInteractable->InteractionFlag);
  this->FocusedInteractable->OnInteractionEnd.RemoveDynamic(this, &UInteractingComponent::HandleInteractionEnded);
  this->OnInteractionEnd.Broadcast(interactingComponent, interactedComponent);
}
void UInteractingComponent::CallEndInteractionOnInteractable() const
{
  if (this->FocusedInteractable)
  {
    this->FocusedInteractable->EndInteraction();
  }
}