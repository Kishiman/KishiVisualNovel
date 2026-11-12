#include "Components/LayeredSpriteWidgetComponent.h"

#include "Assets/LayeredSprite.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "Interfaces/LayeredSpriteHandler.h"
#include "Kismet/KismetMathLibrary.h"

void ULayeredSpriteWidgetComponent::InitWidget()
{
	const UUserWidget *PreviousWidget = GetUserWidgetObject();
	Super::InitWidget();
	// InitWidget runs more than once and keeps an existing widget. Only a new one needs the sprite.
	if (GetUserWidgetObject() != PreviousWidget)
		ApplySprite();
}

void ULayeredSpriteWidgetComponent::BeginPlay()
{
	Super::BeginPlay();
	const UCameraComponent *Camera = GetOwner() ? GetOwner()->FindComponentByClass<UCameraComponent>() : nullptr;
	if (Camera)
		SetWorldRotation(FRotator(0.0f, UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), Camera->GetComponentLocation()).Yaw, 0.0f));
}

void ULayeredSpriteWidgetComponent::SetLayeredSprite(ULayeredSprite *InLayeredSprite)
{
	LayeredSprite = InLayeredSprite;
	ApplySprite();
}

void ULayeredSpriteWidgetComponent::ApplySprite()
{
	UUserWidget *SpriteWidget = GetUserWidgetObject();
	if (!IsValid(SpriteWidget) || !IsValid(LayeredSprite))
		return;
	if (SpriteWidget->GetClass()->ImplementsInterface(ULayeredSpriteHandler::StaticClass()))
		ILayeredSpriteHandler::Execute_SetSprite(SpriteWidget, LayeredSprite);
	else
		UE_LOG(LogTemp, Error, TEXT("%s on %s: widget %s does not implement ILayeredSpriteHandler."), *GetName(), *GetNameSafe(GetOwner()), *GetNameSafe(SpriteWidget->GetClass()));
}
