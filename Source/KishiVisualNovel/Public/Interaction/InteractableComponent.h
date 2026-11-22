#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/SceneComponent.h"
#include "InteractableComponent.generated.h"

class UInteractingComponent;
class UUserWidget;

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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionStart, FName, ActionName, UInteractingComponent *, InstigatingComponent, UInteractableComponent *, InstigatedComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionEnd, UInteractingComponent *, InstigatingComponent, UInteractableComponent *, InstigatedComponent);

/**
 * Makes its owner interactable. It holds the actions on offer, runs the handler registered for the requested action, and shows the prompt widget
 * at its location while focused. An actor has one of these and any number of handlers, such as "talk" + "shop".
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent), BlueprintType)
class KISHIVISUALNOVEL_API UInteractableComponent : public USceneComponent
{
  GENERATED_BODY()

public:
  UInteractableComponent();

  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

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

  /** The actions on offer: these entries plus one per registered handler. When empty, any action is accepted. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  TArray<FInteractableActorAction> Actions;

  UPROPERTY(EditAnywhere, Category = "Interaction")
  FName InteractionFlag = FName("Default");

  /** Prompt widget shown while focused. It should implement IInteractionPromptWidget. Leave unset for no prompt. */
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
  TSubclassOf<UUserWidget> InteractionWidgetClass;

  UPROPERTY(BlueprintReadOnly, Category = "Interaction")
  bool bIsFocused = false;

  UPROPERTY(BlueprintReadOnly, Category = "Interaction")
  bool bIsNearby = false;

  UPROPERTY(BlueprintReadOnly, Category = "Interaction")
  UInteractingComponent *CurrentInteractingComponent = nullptr;

  UFUNCTION(BlueprintPure, Category = "Interaction")
  AActor *GetInteractingActor() const;

  /** True when Interact would accept this request. */
  UFUNCTION(BlueprintPure, Category = "Interaction")
  bool CanInteract(FName ActionName, const UInteractingComponent *InteractingComponent) const;

  UFUNCTION(BlueprintCallable, Category = "Interaction")
  virtual void Interact(FName ActionName, UInteractingComponent *InteractingComponent);

  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void SetIsFocused(bool bFocused);

  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void SetIsNearby(bool bNearby);

  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction")
  TArray<FInteractableActorAction> GetAvailableActions() const { return Actions; }

  UFUNCTION(BlueprintCallable, Category = "Interaction")
  virtual void EndInteraction();

  /**
   * Offers ActionName and makes Handler, which must implement IInteractable, run it. An Actions entry with the same name keeps its place and
   * takes Label, unless Label is empty. An action has one handler: a second registration logs an error and is ignored.
   */
  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void RegisterAction(FName ActionName, const FText &Label, UObject *Handler);

  /** The handler running the current interaction, or None. */
  UFUNCTION(BlueprintPure, Category = "Interaction")
  UObject *GetActiveHandler() const { return ActiveHandler; }

  UFUNCTION(BlueprintPure, Category = "Interaction")
  UUserWidget *GetInteractionWidget() const { return InteractionWidget; }

protected:
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  UPROPERTY(Transient, BlueprintReadOnly, Category = "Interaction")
  UUserWidget *InteractionWidget = nullptr;

private:
  /** The handler registered for ActionName, else the owner or a component that implements IInteractable without registering. */
  UObject *FindHandlerForAction(FName ActionName) const;
  void ShowPrompt();
  void RemovePrompt();
  void UpdatePromptPosition();

  UPROPERTY(Transient)
  TMap<FName, UObject *> ActionHandlers;
  UPROPERTY(Transient)
  UObject *ActiveHandler = nullptr;
};
