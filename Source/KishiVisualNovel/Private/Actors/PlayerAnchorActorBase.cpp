#include "Actors/PlayerAnchorActorBase.h"

#include "Components/SceneComponent.h"
#include "GameFramework/Character.h"

APlayerAnchorActorBase::APlayerAnchorActorBase()
{
	PrimaryActorTick.bCanEverTick = false;
	AnchorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = AnchorRoot;
}

void APlayerAnchorActorBase::OnEnterAnchor_Implementation(ACharacter* PlayerCharacter)
{
	OnAnchorEntered(PlayerCharacter);
}

void APlayerAnchorActorBase::OnExitAnchor_Implementation(ACharacter* PlayerCharacter)
{
	OnAnchorExited(PlayerCharacter);
}

FTransform APlayerAnchorActorBase::GetAnchorTransform_Implementation() const
{
	return PlayerAnchorTransform * GetActorTransform();
}

void APlayerAnchorActorBase::GetViewLimits_Implementation(float& OutMinYaw, float& OutMaxYaw, float& OutMinPitch, float& OutMaxPitch) const
{
	OutMinYaw = MinYaw;
	OutMaxYaw = MaxYaw;
	OutMinPitch = MinPitch;
	OutMaxPitch = MaxPitch;
}

bool APlayerAnchorActorBase::CanAnchor_Implementation(ACharacter* PlayerCharacter) const
{
	return bAnchorAvailable && IsValid(PlayerCharacter);
}
