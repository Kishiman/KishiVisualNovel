#include "Components/RpyCloseVNEventListenerComponent.h"

#include "Interfaces/RpyMainComponent.h"
#include "Interfaces/RpyStatefulComponent.h"

URpyCloseVNEventListenerComponent::URpyCloseVNEventListenerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URpyCloseVNEventListenerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(AutoRegisterVNHost))
		RegisterWithVNHost(AutoRegisterVNHost);
}

void URpyCloseVNEventListenerComponent::OnCloseVNEvent_Implementation()
{
	OnVNClosed.Broadcast();
}

bool URpyCloseVNEventListenerComponent::RegisterWithVNHost(AActor *VNHostActor)
{
	if (URpyMainComponentLibrary::AddOnCloseVNEventListenerOnActor(VNHostActor, this))
		return true;
	UE_LOG(LogTemp, Error, TEXT("%s: %s has no IRpyMainActor or IRpyMainComponent to listen to."), *GetName(), *GetNameSafe(VNHostActor));
	return false;
}

TSubclassOf<URpyState> URpyCloseVNEventListenerComponent::GetStateClass_Implementation()
{
	return URpyStatefulComponentState::StaticClass();
}

URpyState *URpyCloseVNEventListenerComponent::SaveToState_Implementation(UObject *Outer)
{
	URpyStatefulComponentState *State = NewObject<URpyStatefulComponentState>(Outer ? Outer : this);
	if (State)
		State->ComponentName = GetFName();
	return State;
}
