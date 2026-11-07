#include "Components/PlayerAnchorComponent.h"

#include "GameFramework/Character.h"

UPlayerAnchorComponent::UPlayerAnchorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerAnchorComponent::OnEnterAnchor_Implementation(ACharacter *PlayerCharacter)
{
	AnchoredCharacter = PlayerCharacter;
	OnAnchorEntered.Broadcast(PlayerCharacter);
}

void UPlayerAnchorComponent::OnExitAnchor_Implementation(ACharacter *PlayerCharacter)
{
	if (AnchoredCharacter == PlayerCharacter)
		AnchoredCharacter = nullptr;
	OnAnchorExited.Broadcast(PlayerCharacter);
}

FTransform UPlayerAnchorComponent::GetAnchorTransform_Implementation() const
{
	return PlayerAnchorTransform * GetComponentTransform();
}

void UPlayerAnchorComponent::GetViewLimits_Implementation(float &OutMinYaw, float &OutMaxYaw, float &OutMinPitch, float &OutMaxPitch) const
{
	OutMinYaw = MinYaw;
	OutMaxYaw = MaxYaw;
	OutMinPitch = MinPitch;
	OutMaxPitch = MaxPitch;
}

bool UPlayerAnchorComponent::CanAnchor_Implementation(ACharacter *PlayerCharacter) const
{
	if (!bAnchorAvailable || !IsValid(PlayerCharacter))
		return false;
	return !bSingleOccupant || !IsValid(AnchoredCharacter) || AnchoredCharacter == PlayerCharacter;
}
