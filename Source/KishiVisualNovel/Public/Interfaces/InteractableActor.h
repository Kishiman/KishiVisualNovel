#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableActor.generated.h"

/** Simple struct describing an available action */
USTRUCT(BlueprintType)
struct FInteractableActorAction
{
  GENERATED_BODY()

  /** The input action or key bound to this interaction */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  FName ActionName = NAME_None;

  /** Display name for the UI (e.g. "Talk", "Open", "Inspect") */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  FText Label;
};

/**
 * Interface for all interactable actors (items, doors, NPCs, etc.)
 */
UINTERFACE(Blueprintable)
class KISHIVISUALNOVEL_API UInteractableActor : public UInterface
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IInteractableActor
{
  GENERATED_BODY()

public:
  /** Display name for UI */
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
  FText GetInteractableName() const;

  /** List of available actions (button + label) */
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
  TArray<FInteractableActorAction> GetAvailableActions() const;

  /** Called when player performs an interaction (e.g. presses key) */
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
  void OnAction(FName ActionName);

  /** Called when player’s focus raycast hits/leaves this actor */
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
  void SetIsBeingFocused(bool bFocused);

  /** Called when player enters/exits nearby area (sphere trace) */
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
  void SetIsBeingNearby(bool bNearby);
};

UCLASS()
class KISHIVISUALNOVEL_API UInteractableActorLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable)
  static void UpdateInteractableActors(UObject *WorldContextObject, float FocusDistance, float NearbyRadius, TScriptInterface<IInteractableActor> PreviousFocusedActor, TArray<TScriptInterface<IInteractableActor>> PreviousNearbyActors, TScriptInterface<IInteractableActor> &OutFocusedActor, TArray<TScriptInterface<IInteractableActor>> &OutNearbyActors);

protected:
private:
};