// PlayerAnchorComponent.cpp

#include "Components/PlayerAnchorComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"

UPlayerAnchorComponent::UPlayerAnchorComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    bTickInEditor = false;
}

void UPlayerAnchorComponent::Interact(FName ActionName, UInteractingComponent *InteractingComponent)
{
    Super::Interact(ActionName, InteractingComponent);
    AnchoredPlayer = Cast<ACharacter>(InteractingComponent->GetOwner());
    if (!AnchoredPlayer.IsValid())
        return;

    auto PC = Cast<APlayerController>(AnchoredPlayer->GetController());
    if (!PC)
        return;

    // Push mode
    // PC->PushInteractionMode(AnchorInteractionMode);

    // Find camera in owner or children
    CameraRef = GetOwner()->FindComponentByClass<UCameraComponent>();

    // Snap player to anchor transform
    // (Use your previous GetAnchorTransform() logic here)
}

void UPlayerAnchorComponent::EndInteraction()
{
    Super::EndInteraction();
    // auto PC = Cast<APlayerController>(this->GetPlayerController(0));
    // if (PC)
    //     PC->PopInteractionMode();

    AnchoredPlayer = nullptr;
    CameraRef = nullptr;
}

void UPlayerAnchorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!AnchoredPlayer.IsValid() || !CameraRef)
        return;

    // Here call your camera update logic:
    // UPlayerAnchorLibrary::UpdateAnchoredCameraAndCursor(...)
}
