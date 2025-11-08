#include "Utils/RaycastUtils.h"

#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"

AActor *URaycastUtils::RaycastForInterface(
    UObject *WorldContextObject,
    FVector Start,
    FVector End,
    TSubclassOf<UInterface> InterfaceClass,
    FHitResult &OutHit,
    ECollisionChannel TraceChannel,
    bool bDrawDebug,
    float DebugDuration)
{
  if (!WorldContextObject)
    return nullptr;

  UWorld *World = WorldContextObject->GetWorld();
  if (!World)
    return nullptr;

  FCollisionQueryParams Params(SCENE_QUERY_STAT(InteractTrace), false);
  Params.bReturnPhysicalMaterial = false;

  bool bHit = World->LineTraceSingleByChannel(OutHit, Start, End, TraceChannel, Params);

  if (bDrawDebug)
  {
    FColor Color = bHit ? FColor::Green : FColor::Red;
    DrawDebugLine(World, Start, End, Color, false, DebugDuration, 0, 1.0f);
    if (bHit)
      DrawDebugPoint(World, OutHit.ImpactPoint, 10.0f, Color, false, DebugDuration);
  }

  if (bHit && OutHit.GetActor())
  {
    AActor *HitActor = OutHit.GetActor();
    if (HitActor->GetClass()->ImplementsInterface(InterfaceClass))
    {
      return HitActor;
    }
  }

  return nullptr;
}

UActorComponent *URaycastUtils::RaycastForComponent(UObject *WorldContextObject, FVector Start, FVector End, TSubclassOf<UActorComponent> ComponentClass, FHitResult &OutHit, ECollisionChannel TraceChannel, bool bDrawDebug, float DebugDuration)
{
  if (!WorldContextObject)
    return nullptr;

  UWorld *World = WorldContextObject->GetWorld();
  if (!World)
    return nullptr;

  FCollisionQueryParams Params(SCENE_QUERY_STAT(InteractTrace), false);
  Params.bReturnPhysicalMaterial = false;

  bool bHit = World->LineTraceSingleByChannel(OutHit, Start, End, TraceChannel, Params);

  if (bDrawDebug)
  {
    FColor Color = bHit ? FColor::Green : FColor::Red;
    DrawDebugLine(World, Start, End, Color, false, DebugDuration, 0, 1.0f);
    if (bHit)
      DrawDebugPoint(World, OutHit.ImpactPoint, 10.0f, Color, false, DebugDuration);
  }

  if (bHit && OutHit.GetActor())
  {
    AActor *HitActor = OutHit.GetActor();
    auto Component = HitActor->GetComponentByClass(ComponentClass);
    if (Component)
    {
      return Component;
    }
  }
  return nullptr;
}
TArray<AActor *> URaycastUtils::SphereTraceForInterfaces(
    UObject *WorldContextObject,
    FVector Start,
    FVector End,
    float Radius,
    TSubclassOf<UInterface> InterfaceClass,
    TArray<FHitResult> &OutHits,
    ECollisionChannel TraceChannel,
    bool bDrawDebug,
    float DebugDuration)
{
  TArray<AActor *> InteractableActors;
  OutHits.Empty();

  if (!WorldContextObject)
    return InteractableActors;

  UWorld *World = WorldContextObject->GetWorld();
  if (!World)
    return InteractableActors;

  FCollisionQueryParams Params(SCENE_QUERY_STAT(SphereInteractTrace), false);
  Params.bReturnPhysicalMaterial = false;

  bool bHit = World->SweepMultiByChannel(
      OutHits,
      Start,
      End,
      FQuat::Identity,
      TraceChannel,
      FCollisionShape::MakeSphere(Radius),
      Params);

  if (bDrawDebug)
  {
    FColor Color = bHit ? FColor::Cyan : FColor::Red;
    DrawDebugSphere(World, End, Radius, 16, Color, false, DebugDuration);
    DrawDebugLine(World, Start, End, Color, false, DebugDuration, 0, 1.0f);
  }

  if (bHit)
  {
    for (const FHitResult &Hit : OutHits)
    {
      AActor *HitActor = Hit.GetActor();
      if (HitActor && HitActor->GetClass()->ImplementsInterface(InterfaceClass))
      {
        InteractableActors.AddUnique(HitActor);
      }
    }
  }

  return InteractableActors;
}

TArray<UActorComponent *> URaycastUtils::SphereTraceForComponents(UObject *WorldContextObject, FVector Start, FVector End, float Radius, TSubclassOf<UActorComponent> ComponentClass, TArray<FHitResult> &OutHits, ECollisionChannel TraceChannel, bool bDrawDebug, float DebugDuration)
{
  TArray<UActorComponent *> HitComponents;

  if (!WorldContextObject)
    return HitComponents;

  UWorld *World = WorldContextObject->GetWorld();
  if (!World)
    return HitComponents;

  FCollisionQueryParams Params(SCENE_QUERY_STAT(SphereInteractTrace), false);
  Params.bReturnPhysicalMaterial = false;

  bool bHit = World->SweepMultiByChannel(
      OutHits,
      Start,
      End,
      FQuat::Identity,
      TraceChannel,
      FCollisionShape::MakeSphere(Radius),
      Params);

  if (bDrawDebug)
  {
    FColor Color = bHit ? FColor::Cyan : FColor::Red;
    DrawDebugSphere(World, End, Radius, 16, Color, false, DebugDuration);
    DrawDebugLine(World, Start, End, Color, false, DebugDuration, 0, 1.0f);
  }

  if (bHit)
  {
    for (const FHitResult &Hit : OutHits)
    {
      AActor *HitActor = Hit.GetActor();
      if (HitActor)
      {
        auto Component = HitActor->GetComponentByClass(ComponentClass);
        if (Component)
        {
          HitComponents.AddUnique(Component);
        }
      }
    }
  }

  return HitComponents;
}

void URaycastUtils::GetPlayerViewTrace(UObject *WorldContextObject, float TraceDistance, FVector &OutStart, FVector &OutEnd)
{
  OutStart = FVector::ZeroVector;
  OutEnd = FVector::ZeroVector;

  if (!WorldContextObject)
    return;

  APlayerController *PC = WorldContextObject->GetWorld() ? WorldContextObject->GetWorld()->GetFirstPlayerController() : nullptr;
  if (!PC)
    return;

  FVector ViewLoc;
  FRotator ViewRot;
  PC->GetPlayerViewPoint(ViewLoc, ViewRot);

  OutStart = ViewLoc;
  OutEnd = ViewLoc + (ViewRot.Vector() * TraceDistance);
}

AActor *URaycastUtils::RaycastForInterfaceFromPlayer(UObject *WorldContextObject, float TraceDistance, TSubclassOf<UInterface> InterfaceClass, FHitResult &OutHit, ECollisionChannel TraceChannel, bool bDrawDebug, float DebugDuration)
{
  FVector Start, End;
  GetPlayerViewTrace(WorldContextObject, TraceDistance, Start, End);
  return RaycastForInterface(WorldContextObject, Start, End, InterfaceClass, OutHit, TraceChannel, bDrawDebug, DebugDuration);
}

UActorComponent *URaycastUtils::RaycastForComponentFromPlayer(UObject *WorldContextObject, float TraceDistance, TSubclassOf<UActorComponent> ComponentClass, FHitResult &OutHit, ECollisionChannel TraceChannel, bool bDrawDebug, float DebugDuration)
{
  FVector Start, End;
  GetPlayerViewTrace(WorldContextObject, TraceDistance, Start, End);
  return RaycastForComponent(WorldContextObject, Start, End, ComponentClass, OutHit, TraceChannel, bDrawDebug, DebugDuration);
}

TArray<AActor *> URaycastUtils::SphereTraceForInterfacesFromPlayer(UObject *WorldContextObject, float Radius, TSubclassOf<UInterface> InterfaceClass, TArray<FHitResult> &OutHits, ECollisionChannel TraceChannel, bool bDrawDebug, float DebugDuration)
{
  FVector Start, End;
  GetPlayerViewTrace(WorldContextObject, Radius, Start, End);
  // stationary sphere trace at Start location
  return SphereTraceForInterfaces(WorldContextObject, Start, Start, Radius, InterfaceClass, OutHits, TraceChannel, bDrawDebug, DebugDuration);
}

TArray<UActorComponent *> URaycastUtils::SphereTraceForComponentsFromPlayer(UObject *WorldContextObject, float Radius, TSubclassOf<UActorComponent> ComponentClass, TArray<FHitResult> &OutHits, ECollisionChannel TraceChannel, bool bDrawDebug, float DebugDuration)
{
  FVector Start, End;
  GetPlayerViewTrace(WorldContextObject, Radius, Start, End);
  // stationary sphere trace at Start location
  return SphereTraceForComponents(WorldContextObject, Start, Start, Radius, ComponentClass, OutHits, TraceChannel, bDrawDebug, DebugDuration);
}
