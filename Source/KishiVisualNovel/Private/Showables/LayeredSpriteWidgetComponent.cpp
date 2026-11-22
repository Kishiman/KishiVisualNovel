#include "Showables/LayeredSpriteWidgetComponent.h"

#include "Showables/LayeredSprite.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "Showables/LayeredSpriteHandler.h"
#include "Kismet/KismetMathLibrary.h"
#include "Showables/KishiLayeredSpriteWidget.h"

ULayeredSpriteWidgetComponent::ULayeredSpriteWidgetComponent(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
	WidgetClass = UKishiLayeredSpriteWidget::StaticClass();
}

void ULayeredSpriteWidgetComponent::InitWidget()
{
	const UUserWidget *PreviousWidget = GetUserWidgetObject();
	Super::InitWidget();
	// InitWidget runs more than once and keeps an existing widget. Only a new one needs the sprite.
	if (GetUserWidgetObject() != PreviousWidget)
		ApplySprite();
}

void ULayeredSpriteWidgetComponent::OnRegister()
{
	ApplyPixelsPerUnrealUnit();
	Super::OnRegister();
}

#if WITH_EDITOR
void ULayeredSpriteWidgetComponent::PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent)
{
	// Applied on the template too, so the Details panel shows the resulting scale.
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ULayeredSpriteWidgetComponent, PixelsPerUnrealUnit))
		ApplyPixelsPerUnrealUnit();
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void ULayeredSpriteWidgetComponent::ApplyPixelsPerUnrealUnit()
{
	if (PixelsPerUnrealUnit > 0.0f)
		SetRelativeScale3D(FVector(1.0f / PixelsPerUnrealUnit));
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
	if (!IsValid(LayeredSprite))
		return;
	if (!IsValid(SpriteWidget))
	{
		// The widget is created at registration; only a missing Widget Class leaves it empty for good.
		if (!WidgetClass && IsRegistered())
			UE_LOG(LogTemp, Error, TEXT("%s on %s: Widget Class is None, so layered sprite %s is not shown. Set Widget Class to KishiLayeredSpriteWidget."),
						 *GetName(), *GetNameSafe(GetOwner()), *LayeredSprite->GetName());
		return;
	}
	if (SpriteWidget->GetClass()->ImplementsInterface(ULayeredSpriteHandler::StaticClass()))
	{
		ILayeredSpriteHandler::Execute_SetSprite(SpriteWidget, LayeredSprite);
		// With Manually Redraw on, the render target keeps its old image until a redraw is requested.
		RequestRedraw();
	}
	else
		UE_LOG(LogTemp, Error, TEXT("%s on %s: widget %s does not implement ILayeredSpriteHandler."), *GetName(), *GetNameSafe(GetOwner()), *GetNameSafe(SpriteWidget->GetClass()));
}
