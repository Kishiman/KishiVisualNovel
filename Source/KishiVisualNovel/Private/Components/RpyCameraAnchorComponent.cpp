#include "Components/RpyCameraAnchorComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"

URpyCameraAnchorComponent::URpyCameraAnchorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

AActor *URpyCameraAnchorComponent::GetActor_Implementation() const
{
	return GetOwner();
}

UCameraComponent *URpyCameraAnchorComponent::GetCameraComponent_Implementation() const
{
	if (IsValid(Camera))
		return Camera;
	TArray<USceneComponent *> Children;
	GetChildrenComponents(true, Children);
	for (USceneComponent *Child : Children)
		if (UCameraComponent *ChildCamera = Cast<UCameraComponent>(Child))
			return ChildCamera;
	return GetOwner() ? GetOwner()->FindComponentByClass<UCameraComponent>() : nullptr;
}

FName URpyCameraAnchorComponent::GetAnchorLayer_Implementation() const
{
	return AnchorLayer;
}
