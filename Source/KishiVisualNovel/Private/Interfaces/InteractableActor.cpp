#include "Interfaces/InteractableActor.h"
#include "Utils/ArrayUtils.h"
#include "Utils/RayCastUtils.h"

void UInteractableActorLibrary::UpdateInteractableActors(UObject *WorldContextObject, float FocusDistance, float NearbyRadius, TScriptInterface<IInteractableActor> PreviousFocusedActor, TArray<TScriptInterface<IInteractableActor>> PreviousNearbyActors, TScriptInterface<IInteractableActor> &OutFocusedActor, TArray<TScriptInterface<IInteractableActor>> &OutNearbyActors)
{
  OutFocusedActor = nullptr;
  OutNearbyActors.Empty();

  if (!WorldContextObject)
    return;

  FHitResult FocusHit;
  AActor *FocusedActor = URaycastUtils::RaycastForInteractableFromPlayer(
      WorldContextObject,
      FocusDistance,
      UInteractableActor::StaticClass(),
      FocusHit,
      ECC_Visibility,
      false,
      0.0f);

  if (FocusedActor && FocusedActor->GetClass()->ImplementsInterface(UInteractableActor::StaticClass()))
  {
    TScriptInterface<IInteractableActor> FocusedActorInter;
    FocusedActorInter.SetObject(FocusedActor);
    FocusedActorInter.SetInterface(Cast<IInteractableActor>(FocusedActor));
    if (FocusedActorInter.GetObject() != PreviousFocusedActor.GetObject())
    {
      if (PreviousFocusedActor.GetObject())
        IInteractableActor::Execute_SetIsBeingFocused(PreviousFocusedActor.GetObject(), false);
      IInteractableActor::Execute_SetIsBeingFocused(FocusedActorInter.GetObject(), true);
    }
    OutFocusedActor = FocusedActorInter;
  }
  else
  {
    if (PreviousFocusedActor.GetObject())
      IInteractableActor::Execute_SetIsBeingFocused(PreviousFocusedActor.GetObject(), false);
    OutFocusedActor = nullptr;
  }

  TArray<FHitResult> OutHits;
  TArray<AActor *> NearbyActors = URaycastUtils::SphereTraceForInteractablesForInteractableFromPlayer(
      WorldContextObject,
      NearbyRadius,
      UInteractableActor::StaticClass(),
      OutHits,
      ECC_Pawn,
      false,
      0.0f);

  for (AActor *NearbyActor : NearbyActors)
  {
    if (NearbyActor->GetClass()->ImplementsInterface(UInteractableActor::StaticClass()))
    {
      TScriptInterface<IInteractableActor> NearbyActorInter;
      NearbyActorInter.SetObject(NearbyActor);
      NearbyActorInter.SetInterface(Cast<IInteractableActor>(NearbyActor));
      OutNearbyActors.Add(NearbyActorInter);
    }
  }
  TArray<TScriptInterface<IInteractableActor>> ExistingActorsInter;
  TArray<TScriptInterface<IInteractableActor>> RemovedActorsInter;
  TArray<TScriptInterface<IInteractableActor>> AddedActorsInter;

  UArrayUtils::CompareArraysTyped<TScriptInterface<IInteractableActor>>(
      PreviousNearbyActors,
      OutNearbyActors,
      /*Existing=*/ExistingActorsInter,
      /*Removed=*/RemovedActorsInter,
      /*Added=*/AddedActorsInter);
  for (TScriptInterface<IInteractableActor> &RemovedActorInter : RemovedActorsInter)
  {
    IInteractableActor::Execute_SetIsBeingNearby(RemovedActorInter.GetObject(), false);
  }
  for (TScriptInterface<IInteractableActor> &AddedActorInter : AddedActorsInter)
  {
    IInteractableActor::Execute_SetIsBeingNearby(AddedActorInter.GetObject(), true);
  }
}