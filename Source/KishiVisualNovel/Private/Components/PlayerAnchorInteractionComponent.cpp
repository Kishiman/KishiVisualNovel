#include "Components/PlayerAnchorInteractionComponent.h"

#include "Components/InputComponent.h"
#include "Components/InteractingComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/PlayerAnchor.h"
#include "Utils/PlayerControllerUtils.h"

UPlayerAnchorInteractionComponent::UPlayerAnchorInteractionComponent()
{
	BlendTime = 0.1f;
}

void UPlayerAnchorInteractionComponent::HandleInteractionStart_Implementation(FName ActionName, UInteractingComponent *InteractingComponent, UInteractableComponent *InteractableComponent)
{
	ACharacter *Character = Cast<ACharacter>(InteractingComponent->GetOwner());
	UObject *Anchor = Character ? UPlayerAnchorLibrary::FindPlayerAnchor(GetOwner()).GetObject() : nullptr;
	if (Anchor && !IPlayerAnchor::Execute_CanAnchor(Anchor, Character))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s on %s: refused because %s returned false from CanAnchor."), *GetName(), *GetNameSafe(GetOwner()), *GetNameSafe(Anchor));
		InteractableComponent->EndInteraction();
		return;
	}
	bAnchored = true;
	if (Anchor)
		IPlayerAnchor::Execute_OnEnterAnchor(Anchor, Character);

	APlayerController *PlayerController = FindInteractingController(InteractingComponent);
	// Game and UI, because the Return action is game input.
	UPlayerControllerUtils::SetPlayerInputMode(PlayerController, EPlayerInputMode::GameAndUI);
	EnterCameraView(InteractingComponent);

	if (!PlayerController)
		return;
	if (!ReturnInput)
	{
		ReturnInput = NewObject<UInputComponent>(GetOwner());
		ReturnInput->RegisterComponent();
		ReturnInput->BindAction(ReturnActionName, IE_Pressed, this, &UPlayerAnchorInteractionComponent::HandleReturnPressed);
	}
	PlayerController->PushInputComponent(ReturnInput);
	ListeningController = PlayerController;
}

void UPlayerAnchorInteractionComponent::HandleInteractionEnd_Implementation(UInteractingComponent *InteractingComponent, UInteractableComponent *InteractableComponent)
{
	if (!bAnchored)
		return;
	bAnchored = false;
	ACharacter *Character = InteractingComponent ? Cast<ACharacter>(InteractingComponent->GetOwner()) : nullptr;
	if (UObject *Anchor = Character ? UPlayerAnchorLibrary::FindPlayerAnchor(GetOwner()).GetObject() : nullptr)
		IPlayerAnchor::Execute_OnExitAnchor(Anchor, Character);
	LeaveCameraView(InteractingComponent);
	StopListeningForReturn();
	UPlayerControllerUtils::SetPlayerInputMode(FindInteractingController(InteractingComponent), EPlayerInputMode::Gameplay);
}

void UPlayerAnchorInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopListeningForReturn();
	Super::EndPlay(EndPlayReason);
}

void UPlayerAnchorInteractionComponent::HandleReturnPressed()
{
	if (bEndInteractionOnInput && IsValid(Interactable))
		Interactable->EndInteraction();
}

void UPlayerAnchorInteractionComponent::StopListeningForReturn()
{
	if (IsValid(ListeningController) && ReturnInput)
		ListeningController->PopInputComponent(ReturnInput);
	ListeningController = nullptr;
}
