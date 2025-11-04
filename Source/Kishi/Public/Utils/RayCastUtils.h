#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RaycastUtils.generated.h"

/**
 * Utility library for line and sphere traces to detect interactable actors.
 * Works with any actor implementing a custom interface (e.g., IInteractableInterface).
 */
UCLASS()
class KISHI_API URaycastUtils : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()

public:
  /** Simple line trace from the camera or any start location, returning first interactable actor hit. */
  UFUNCTION(BlueprintCallable, Category = "Raycast|Interaction")
  static AActor *RaycastForInteractable(
      UObject *WorldContextObject,
      FVector Start,
      FVector End,
      TSubclassOf<UInterface> InteractableInterface,
      FHitResult &OutHit,
      ECollisionChannel TraceChannel = ECC_Visibility,
      bool bDrawDebug = false,
      float DebugDuration = 2.0f);

  /** Sphere trace for interactable actors within a radius (useful for area-based interaction). */
  UFUNCTION(BlueprintCallable, Category = "Raycast|Interaction")
  static TArray<AActor *> SphereTraceForInteractables(
      UObject *WorldContextObject,
      FVector Start,
      FVector End,
      float Radius,
      TSubclassOf<UInterface> InteractableInterface,
      TArray<FHitResult> &OutHits,
      ECollisionChannel TraceChannel = ECC_Visibility,
      bool bDrawDebug = false,
      float DebugDuration = 2.0f);

  /** Utility: get the trace start and end based on a player's camera direction. */
  UFUNCTION(BlueprintPure, Category = "Raycast|Helpers")
  static void GetPlayerViewTrace(UObject *WorldContextObject, float TraceDistance, FVector &OutStart, FVector &OutEnd);

  UFUNCTION(BlueprintPure, Category = "Raycast|Helpers")
  static AActor *RaycastForInteractableFromPlayer(UObject *WorldContextObject,
                                                  float TraceDistance,
                                                  TSubclassOf<UInterface> InteractableInterface,
                                                  FHitResult &OutHit,
                                                  ECollisionChannel TraceChannel = ECC_Visibility,
                                                  bool bDrawDebug = false,
                                                  float DebugDuration = 2.0f);

  UFUNCTION(BlueprintPure, Category = "Raycast|Helpers")
  static TArray<AActor *> SphereTraceForInteractablesForInteractableFromPlayer(UObject *WorldContextObject,
                                                                               float Radius,
                                                                               TSubclassOf<UInterface> InteractableInterface,
                                                                               TArray<FHitResult> &OutHits,
                                                                               ECollisionChannel TraceChannel = ECC_Visibility,
                                                                               bool bDrawDebug = false,
                                                                               float DebugDuration = 2.0f);
};
