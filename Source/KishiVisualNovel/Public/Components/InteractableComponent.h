#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/ActorComponent.h"
#include "InteractableComponent.generated.h"

class UInteractingComponent;

UENUM(BlueprintType)
enum class EInteractableState : uint8
{
  Interactable UMETA(DisplayName = "Interactable"),
  Interacting UMETA(DisplayName = "Interacting"),
  Disabled UMETA(DisplayName = "Disabled")
};

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocusChanged, bool, bFocused);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNearbyChanged, bool, bNearby);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionStart, FName, ActionName, AActor *, InstigatingActor, AActor *, InstigatedActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionEnd);

UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent), BlueprintType)
class KISHIVISUALNOVEL_API UInteractableComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  UInteractableComponent();

  // Delegates that other systems listen to
  UPROPERTY(BlueprintAssignable, Category = "Interaction")
  FOnFocusChanged OnFocusChanged;
  UPROPERTY(BlueprintAssignable, Category = "Interaction")
  FOnNearbyChanged OnNearbyChanged;
  UPROPERTY(BlueprintAssignable, Category = "Interaction")
  FOnInteractionStart OnInteractionStart;

  UPROPERTY(BlueprintAssignable, Category = "Interaction")
  FOnInteractionEnd OnInteractionEnd;

  // State
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  FText InteractableName;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  EInteractableState State = EInteractableState::Interactable;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  TArray<FInteractableActorAction> Actions;

  UPROPERTY(BlueprintReadOnly, Category = "Interaction")
  bool bIsFocused = false;

  UPROPERTY(BlueprintReadOnly, Category = "Interaction")
  bool bIsNearby = false;

public:
  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void SetState(EInteractableState NewState);

  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void Interact(FName ActionName, UInteractingComponent *interactingComponent);

  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void SetIsFocused(bool bFocused);

  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void SetIsNearby(bool bNearby);

  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction")
  TArray<FInteractableActorAction> GetAvailableActions() const { return Actions; }

  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void EndInteraction() const;
};
