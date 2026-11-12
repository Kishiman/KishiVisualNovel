#include "Components/InteractionHandlerComponentBase.h"

#include "Camera/CameraComponent.h"
#include "Components/CameraAnchorComponent.h"
#include "Components/InteractingComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

UInteractionHandlerComponentBase::UInteractionHandlerComponentBase()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionHandlerComponentBase::BeginPlay()
{
	Super::BeginPlay();
	Interactable = UInteractableLibrary::FindOrAddInteractable(GetOwner());
	if (Interactable)
		Interactable->RegisterAction(HandledActionName, HandledActionLabel, this);
}

APlayerController *UInteractionHandlerComponentBase::FindInteractingController(const UInteractingComponent *InteractingComponent) const
{
	if (InteractingComponent)
		if (const APawn *Pawn = Cast<APawn>(InteractingComponent->GetOwner()))
			if (APlayerController *PawnController = Cast<APlayerController>(Pawn->GetController()))
				return PawnController;
	return UGameplayStatics::GetPlayerController(this, 0);
}

void UInteractionCameraHandlerComponentBase::EnterCameraView(UInteractingComponent *InteractingComponent)
{
	AActor *Owner = GetOwner();
	if (!Owner->FindComponentByClass<UCameraComponent>())
		return;
	if (APlayerController *PlayerController = FindInteractingController(InteractingComponent))
		PlayerController->SetViewTargetWithBlend(Owner, BlendTime);
	if (UCameraAnchorComponent *CameraAnchor = Owner->FindComponentByClass<UCameraAnchorComponent>())
		CameraAnchor->Activate();
	bInCameraView = true;
}

void UInteractionCameraHandlerComponentBase::LeaveCameraView(UInteractingComponent *InteractingComponent)
{
	if (!bInCameraView)
		return;
	bInCameraView = false;
	APlayerController *PlayerController = FindInteractingController(InteractingComponent);
	if (PlayerController && InteractingComponent)
		PlayerController->SetViewTargetWithBlend(InteractingComponent->GetOwner(), BlendTime);
	if (UCameraAnchorComponent *CameraAnchor = GetOwner()->FindComponentByClass<UCameraAnchorComponent>())
		CameraAnchor->Deactivate();
}
