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

  if (bIsInteracting)
    return;
  ScanInteractableActors();
}

void UInteractingComponent::ScanInteractableActors()
{
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

  if (focusedInteractable && focusedInteractable->State == EInteractableState::Interactable)
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
  if (this->FocusedInteractable && !this->bIsInteracting)
  {
    this->bIsInteracting = true;
    this->FocusedInteractable->Interact(ActionName, this);
    this->OnInteractionStart.Broadcast(ActionName, GetOwner(), this->FocusedInteractable->GetOwner());
    this->FocusedInteractable->OnInteractionEnd.AddDynamic(this, &UInteractingComponent::HandleInteractionEnded);
    return this->FocusedInteractable;
  }
  return nullptr;
}
void UInteractingComponent::HandleInteractionEnded()
{
  this->bIsInteracting = false;
  this->OnInteractionEnd.Broadcast();
}
void UInteractingComponent::CallEndInteractionOnInteractable() const
{
  if (this->FocusedInteractable)
  {
    this->FocusedInteractable->EndInteraction();
  }
}