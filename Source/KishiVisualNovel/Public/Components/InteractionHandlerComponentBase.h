#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/Interactable.h"
#include "InteractionHandlerComponentBase.generated.h"

class APlayerController;
class UCameraAnchorComponent;
class UCameraComponent;

/**
 * Native starting point for a component that defines what one action does on its owner. At BeginPlay it finds the owner's Interactable component,
 * or adds one, and registers its action there, so an actor can have several handlers with different action names. Subclasses implement HandleInteractionStart.
 * To react to an interaction without adding an action, bind the Interactable component's delegates.
 */
UCLASS(Abstract, Blueprintable, ClassGroup = (Interaction))
class KISHIVISUALNOVEL_API UInteractionHandlerComponentBase : public UActorComponent, public IInteractable
{
	GENERATED_BODY()

public:
	UInteractionHandlerComponentBase();

	virtual void HandleInteractionEnd_Implementation(UInteractingComponent *InteractingComponent, UInteractableComponent *InteractableComponent) override {}

	UFUNCTION(BlueprintPure, Category = "Interaction Handler Component Base")
	UInteractableComponent *GetInteractable() const { return Interactable; }

protected:
	virtual void BeginPlay() override;

	/** The controller of the interacting pawn, else the first player controller. */
	APlayerController *FindInteractingController(const UInteractingComponent *InteractingComponent) const;

	/** Input action that runs this handler. Each handler on an actor needs its own. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction Handler Component Base")
	FName HandledActionName = TEXT("action");
	/** Text the prompt shows for this action, such as "Talk". When empty, an Actions entry's label or else the action name is shown. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction Handler Component Base")
	FText HandledActionLabel;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Interaction Handler Component Base")
	UInteractableComponent *Interactable = nullptr;
};

/**
 * Handler that can move the player's view to its owner during the interaction. The view moves only when the owner has a Camera component.
 * A Camera Anchor component on the owner adds edge-of-screen turning.
 */
UCLASS(Abstract, Blueprintable, ClassGroup = (Interaction))
class KISHIVISUALNOVEL_API UInteractionCameraHandlerComponentBase : public UInteractionHandlerComponentBase
{
	GENERATED_BODY()

protected:
	/** Blends the view to the owner's camera and starts its Camera Anchor. Does nothing when the owner has no camera. */
	void EnterCameraView(UInteractingComponent *InteractingComponent);
	/** Undoes EnterCameraView. Does nothing when the view did not move. */
	void LeaveCameraView(UInteractingComponent *InteractingComponent);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Camera Handler Component Base", meta = (ClampMin = "0"))
	float BlendTime = 0.5f;

private:
	bool bInCameraView = false;
};
