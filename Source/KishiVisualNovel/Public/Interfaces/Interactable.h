#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/Interface.h"
#include "Components/InteractableComponent.h"
#include "Interactable.generated.h"

UINTERFACE(Blueprintable)
class KISHIVISUALNOVEL_API UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Defines what an interaction does. Implement it on the actor that carries the UInteractableComponent, or on a component of that actor.
 * After broadcasting its delegates, the interactable runs the handler registered for the action (UInteractableComponent::RegisterAction), else the
 * first of FindInteractableHandlers that did not register. Focus and nearby hooks go to all of them. UInteractionHandlerComponentBase is the proposed implementation.
 */
class KISHIVISUALNOVEL_API IInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IInteractable|Required", meta = (ToolTip = "Required: run the interaction. Call EndInteraction on InteractableComponent when it is over, which can be immediately."))
	void HandleInteractionStart(FName ActionName, UInteractingComponent *InteractingComponent, UInteractableComponent *InteractableComponent);
	virtual void HandleInteractionStart_Implementation(FName ActionName, UInteractingComponent *InteractingComponent, UInteractableComponent *InteractableComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Required implementation missing: IInteractable::HandleInteractionStart"));
	}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IInteractable|Required", meta = (ToolTip = "Required: undo what HandleInteractionStart set up. The component is already back in the Interactable state."))
	void HandleInteractionEnd(UInteractingComponent *InteractingComponent, UInteractableComponent *InteractableComponent);
	virtual void HandleInteractionEnd_Implementation(UInteractingComponent *InteractingComponent, UInteractableComponent *InteractableComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Required implementation missing: IInteractable::HandleInteractionEnd"));
	}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IInteractable|Hooks", meta = (ToolTip = "Optional: called when the player starts or stops aiming at this interactable. The default does nothing."))
	void OnInteractableFocusChanged(bool bFocused);
	virtual void OnInteractableFocusChanged_Implementation(bool bFocused) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IInteractable|Hooks", meta = (ToolTip = "Optional: called when the player enters or leaves the nearby radius of this interactable. The default does nothing."))
	void OnInteractableNearbyChanged(bool bNearby);
	virtual void OnInteractableNearbyChanged_Implementation(bool bNearby) {}
};

UINTERFACE(Blueprintable)
class KISHIVISUALNOVEL_API UInteractionPromptWidget : public UInterface
{
	GENERATED_BODY()
};

/** Implemented by the on-screen prompt widget that UInteractableComponent shows while it is focused. */
class KISHIVISUALNOVEL_API IInteractionPromptWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IInteractionPromptWidget|Required", meta = (ToolTip = "Required: show DisplayName and one entry per action."))
	void HandleInteractionPromptSetup(const FText &DisplayName, const TArray<FInteractableActorAction> &Actions);
	virtual void HandleInteractionPromptSetup_Implementation(const FText &DisplayName, const TArray<FInteractableActorAction> &Actions)
	{
		UE_LOG(LogTemp, Error, TEXT("Required implementation missing: IInteractionPromptWidget::HandleInteractionPromptSetup"));
	}
};

UCLASS()
class KISHIVISUALNOVEL_API UInteractableLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Returns Actor itself when it implements IInteractable, followed by each of its components that does. */
	UFUNCTION(BlueprintPure, Category = "Interaction")
	static TArray<UObject *> FindInteractableHandlers(AActor *Actor);

	/** Returns Actor's first Interactable component, adding one at the root when it has none. */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	static UInteractableComponent *FindOrAddInteractable(AActor *Actor);
};
