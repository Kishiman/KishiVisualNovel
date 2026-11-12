#include "Components/RpyScriptInteractionComponent.h"

#include "Components/InteractingComponent.h"
#include "Components/RpyCloseVNEventListenerComponent.h"
#include "Interfaces/RpyMainComponent.h"
#include "Rpy/RpyScript.h"

void URpyScriptInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	CloseListener = NewObject<URpyCloseVNEventListenerComponent>(GetOwner());
	CloseListener->RegisterComponent();
	CloseListener->OnVNClosed.AddUniqueDynamic(this, &URpyScriptInteractionComponent::HandleVNClosed);
}

void URpyScriptInteractionComponent::HandleInteractionStart_Implementation(FName ActionName, UInteractingComponent *InteractingComponent, UInteractableComponent *InteractableComponent)
{
	if (!IsValid(InteractionScript))
	{
		UE_LOG(LogTemp, Error, TEXT("%s on %s: InteractionScript is not set."), *GetName(), *GetNameSafe(GetOwner()));
		InteractableComponent->EndInteraction();
		return;
	}

	AActor *InteractingActor = InteractingComponent->GetOwner();
	// Register first: a short script can close the VN before StartRpyScriptOnActor returns.
	if (!CloseListener->RegisterWithVNHost(InteractingActor) || !URpyMainComponentLibrary::StartRpyScriptOnActor(InteractingActor, InteractionScript, InteractionScriptLabel))
	{
		InteractableComponent->EndInteraction();
		return;
	}
	if (InteractableComponent->CurrentInteractingComponent != InteractingComponent)
		return; // The script already finished.
	if (!URpyMainComponentLibrary::IsVNActiveOnActor(InteractingActor))
	{
		UE_LOG(LogTemp, Error, TEXT("%s on %s: %s could not start script %s."), *GetName(), *GetNameSafe(GetOwner()), *GetNameSafe(InteractingActor), *GetNameSafe(InteractionScript));
		InteractableComponent->EndInteraction();
		return;
	}
	EnterCameraView(InteractingComponent);
}

void URpyScriptInteractionComponent::HandleInteractionEnd_Implementation(UInteractingComponent *InteractingComponent, UInteractableComponent *InteractableComponent)
{
	LeaveCameraView(InteractingComponent);
}

void URpyScriptInteractionComponent::HandleVNClosed()
{
	// This listener stays registered after its script, so another handler on the owner may be the one running.
	if (IsValid(Interactable) && Interactable->GetActiveHandler() == this)
		Interactable->EndInteraction();
}
