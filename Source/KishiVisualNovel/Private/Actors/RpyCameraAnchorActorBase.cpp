#include "Actors/RpyCameraAnchorActorBase.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"

ARpyCameraAnchorActorBase::ARpyCameraAnchorActorBase()
{
	PrimaryActorTick.bCanEverTick = false;
	AnchorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = AnchorRoot;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(AnchorRoot);
}

AActor* ARpyCameraAnchorActorBase::GetActor_Implementation() const
{
	return const_cast<ARpyCameraAnchorActorBase*>(this);
}

UCameraComponent* ARpyCameraAnchorActorBase::GetCameraComponent_Implementation() const
{
	return Camera;
}

FName ARpyCameraAnchorActorBase::GetAnchorLayer_Implementation() const
{
	return AnchorLayer;
}
