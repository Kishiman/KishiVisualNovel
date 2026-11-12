#pragma once

#include "CoreMinimal.h"
#include "Components/InteractionHandlerComponentBase.h"
#include "PlayerAnchorInteractionComponent.generated.h"

class APlayerController;
class UInputComponent;

/**
 * Lets the player use a spot on the owner, looking through its Camera, until they press the Return action. Player input stays locked meanwhile.
 * When the owner has an IPlayerAnchor (such as a Player Anchor component), it decides who may enter and is told who enters and leaves.
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent, DisplayName = "Player Anchor Interaction Component"), BlueprintType, Blueprintable)
class KISHIVISUALNOVEL_API UPlayerAnchorInteractionComponent : public UInteractionCameraHandlerComponentBase
{
	GENERATED_BODY()

public:
	UPlayerAnchorInteractionComponent();

	virtual void HandleInteractionStart_Implementation(FName ActionName, UInteractingComponent *InteractingComponent, UInteractableComponent *InteractableComponent) override;
	virtual void HandleInteractionEnd_Implementation(UInteractingComponent *InteractingComponent, UInteractableComponent *InteractableComponent) override;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** End the interaction when ReturnActionName is pressed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Anchor Interaction Component")
	bool bEndInteractionOnInput = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Anchor Interaction Component")
	FName ReturnActionName = TEXT("Return");

private:
	void HandleReturnPressed();
	void StopListeningForReturn();

	/** This component's own input, so it does not touch the input of whatever actor it is on. */
	UPROPERTY(Transient)
	UInputComponent *ReturnInput = nullptr;
	UPROPERTY(Transient)
	APlayerController *ListeningController = nullptr;
	/** False when the interaction ended before anything was set up: a refused anchor. */
	bool bAnchored = false;
};
