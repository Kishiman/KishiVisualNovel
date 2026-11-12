#include "Components/RpyFirstPersonComponent.h"

#include "Blueprint/UserWidget.h"
#include "Components/InputComponent.h"
#include "Components/InteractingComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Widgets/CursorOverlayWidget.h"

URpyFirstPersonComponent::URpyFirstPersonComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	// VNInputMode replaces the parent's Game and UI switch, which leaves the pawn free to move.
	bSetUIInputMode = false;
	InteractActionNames.Add(TEXT("action"));
}

void URpyFirstPersonComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor *Owner = GetOwner();
	if (!Cast<APawn>(Owner))
	{
		UE_LOG(LogTemp, Error, TEXT("%s: %s is not a pawn, so it cannot receive player input."), *GetName(), *GetNameSafe(Owner));
		SetComponentTickEnabled(false);
		return;
	}
	Owner->bFindCameraComponentWhenViewTarget = true;

	Interacting = Owner->FindComponentByClass<UInteractingComponent>();
	if (!Interacting)
	{
		Interacting = NewObject<UInteractingComponent>(Owner);
		Interacting->SetupAttachment(Owner->GetRootComponent());
		Interacting->RegisterComponent();
	}
	Interacting->OnInteractionEnd.AddUniqueDynamic(this, &URpyFirstPersonComponent::HandleInteractionEnded);

	UpdatePlayerBindings();
}

void URpyFirstPersonComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ReleaseVNInput();
	Super::EndPlay(EndPlayReason);
}

void URpyFirstPersonComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdatePlayerBindings();
}

void URpyFirstPersonComponent::UpdatePlayerBindings()
{
	const APawn *Pawn = Cast<APawn>(GetOwner());
	APlayerController *PlayerController = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;
	if (!PlayerController)
		return;

	if (Pawn->InputComponent && Pawn->InputComponent != BoundInputComponent)
	{
		for (const FName &ActionName : InteractActionNames)
		{
			FInputActionBinding Binding(ActionName, IE_Pressed);
			Binding.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &URpyFirstPersonComponent::HandleInteractPressed, ActionName);
			Pawn->InputComponent->AddActionBinding(Binding);
		}
		BoundInputComponent = Pawn->InputComponent;
	}

	if (bPlayerReady)
		return;
	bPlayerReady = true;
	if (CursorWidgetClass)
	{
		CursorWidget = CreateWidget<UCursorOverlayWidget>(PlayerController, CursorWidgetClass);
		if (IsValid(CursorWidget))
			PlayerController->SetMouseCursorWidget(EMouseCursor::Default, CursorWidget);
		else
			UE_LOG(LogTemp, Error, TEXT("%s: could not create cursor widget %s."), *GetName(), *GetNameSafe(CursorWidgetClass));
	}
	// A loaded game may already have restored a session.
	if (IsValid(StartScript) && !IsValid(Session))
		IRpyMainComponent::Execute_StartRpyScript(this, StartScript, StartLabel);
}

void URpyFirstPersonComponent::HandleInteractPressed(FName ActionName)
{
	if (IsValid(Interacting))
		Interacting->CallInteractOnInteractable(ActionName);
}

void URpyFirstPersonComponent::CaptureVNInput()
{
	if (bVNInputCaptured)
		return;
	APlayerController *PlayerController = FindPlayerController();
	if (!PlayerController)
		return;
	UPlayerControllerUtils::SetPlayerInputMode(PlayerController, VNInputMode);
	if (IsValid(VNPlayer))
		VNPlayer->SetUserFocus(PlayerController);
	bVNInputCaptured = true;
}

void URpyFirstPersonComponent::ReleaseVNInput()
{
	if (!bVNInputCaptured)
		return;
	UPlayerControllerUtils::SetPlayerInputMode(FindPlayerController(), EPlayerInputMode::Gameplay);
	bVNInputCaptured = false;
}

void URpyFirstPersonComponent::StartRpyScript_Implementation(URpyScript *Script, FName const &Label)
{
	Super::StartRpyScript_Implementation(Script, Label);
	// The session is gone if the script failed to start or already finished.
	if (IsValid(Session))
		CaptureVNInput();
}

void URpyFirstPersonComponent::LoadFromState_Implementation(const URpyState *State)
{
	Super::LoadFromState_Implementation(State);
	if (IsValid(Session))
		CaptureVNInput();
}

void URpyFirstPersonComponent::CloseVN_Implementation()
{
	Super::CloseVN_Implementation();
	ReleaseVNInput();
	if (IsValid(Interacting))
		Interacting->CallEndInteractionOnInteractable();
	if (IsValid(CursorWidget))
		CursorWidget->CenterOnScreen();
}

void URpyFirstPersonComponent::HandleInteractionEnded(UInteractingComponent *InteractingComponent, UInteractableComponent *InteractableComponent)
{
	if (IsValid(Session))
		return; // The VN still needs the UI input mode.
	bVNInputCaptured = false;
	UPlayerControllerUtils::SetPlayerInputMode(FindPlayerController(), EPlayerInputMode::Gameplay);
}
