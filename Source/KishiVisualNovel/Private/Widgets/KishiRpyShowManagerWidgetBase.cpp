#include "Widgets/KishiRpyShowManagerWidgetBase.h"
#include "Interfaces/RpyScriptInterpreterBaseManager.h"
#include "Interfaces/RpyScriptInterpreter.h"
#include "Interfaces/LayeredSpriteHandler.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "Components/SceneComponent.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"
#include "LevelSequencePlayer.h"
#include "PaperSprite.h"
#include "TimerManager.h"
#include "Rpy/RpySession.h"
#include "Templates/KishiScriptInterface.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"
#include "KishiRpyWidgetReflection.h"
#include "Animation/KishiAnimationComponent.h"

void UKishiRpyShowManagerWidgetBase::SetShowManagerState_Implementation(const FRpyShowManagerState &State)
{
	for (const TPair<FName, UUserWidget *> &Pair : ShownWidgets)
	{
		if (IsValid(Pair.Value))
			Pair.Value->RemoveFromParent();
	}
	ShownWidgets.Reset();
	ShowState = State;
	for (const FRpyShowImageState &ImageState : ShowState.imageStates)
	{
		Show_Implementation(ImageState.rpyImage, ImageState.options);
	}
}

bool UKishiRpyShowManagerWidgetBase::Show_Implementation(FRpyImage Image, FRpySceneOptions Options)
{
	FRpyShowImageState *Existing = ShowState.imageStates.FindByPredicate([&Image](const FRpyShowImageState &Item)
																																			 { return Item.name == Image.tag; });
	if (!Existing)
	{
		Existing = &ShowState.imageStates.AddDefaulted_GetRef();
	}
	Existing->name = Image.tag;
	Existing->rpyImage = Image;
	Existing->options = Options;
	if (ShowCanvas)
	{
		if (FTimerHandle *HideTimer = HideTimers.Find(Image.tag))
		{
			if (GetWorld())
				GetWorld()->GetTimerManager().ClearTimer(*HideTimer);
			HideTimers.Remove(Image.tag);
		}
		RemoveShownWidget(Image.tag);
		if (UUserWidget *ImageWidget = CreateImageWidget(Image, Options))
		{
			ShownWidgets.Add(Image.tag, ImageWidget);
			PlayImageTransition(ImageWidget, Options, 0);
		}
	}
	OnShowRequested(Image, Options);
	return Image.type == ERpyImageType::ESPRITE || Image.type == ERpyImageType::ELAYERED_SPRITE;
}

bool UKishiRpyShowManagerWidgetBase::Hide_Implementation(FName Tag, FRpySceneOptions Options)
{
	const int32 Index = ShowState.imageStates.IndexOfByPredicate([Tag](const FRpyShowImageState &Item)
																															 { return Item.name == Tag || Item.rpyImage.tag == Tag; });
	if (Index == INDEX_NONE)
	{
		return false;
	}
	OnHideRequested(Tag, Options);
	const FName StateTag = ShowState.imageStates[Index].name;
	if (Options.with != ERPYTransitionType::NONE && Options.transitionTime > 0.0f)
	{
		UUserWidget **Widget = ShownWidgets.Find(StateTag);
		if (Widget && IsValid(*Widget) && GetWorld())
		{
			PlayImageTransition(*Widget, Options, 1);
			FTimerHandle &Handle = HideTimers.FindOrAdd(StateTag);
			GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateWeakLambda(this, [this, StateTag]()
																																											{ HideTimers.Remove(StateTag); RemoveShownWidget(StateTag); }),
																						 Options.transitionTime, false);
		}
		else
			RemoveShownWidget(StateTag);
	}
	else
		RemoveShownWidget(StateTag);
	ShowState.imageStates.RemoveAt(Index);
	return true;
}

void UKishiRpyShowManagerWidgetBase::Reset_Implementation()
{
	TArray<FName> GenericTags;
	TrackedShowables.GetKeys(GenericTags);
	for (FName Tag : GenericTags) RemoveShowable(Tag);
	for (const TPair<FName, UUserWidget *> &Pair : ShownWidgets)
	{
		if (IsValid(Pair.Value))
			Pair.Value->RemoveFromParent();
	}
	ShownWidgets.Reset();
	if (GetWorld())
	{
		for (TPair<FName, FTimerHandle> &Pair : HideTimers)
			GetWorld()->GetTimerManager().ClearTimer(Pair.Value);
	}
	HideTimers.Reset();
	ShowState.imageStates.Reset();
	UKishiRpyManagerWidgetBase::Reset_Implementation();
}

UObject* UKishiRpyShowManagerWidgetBase::CreateShowableFromClass(TSubclassOf<UObject> ShowableClass, FName Tag, FVector WorldLocation, FRotator WorldRotation, FVector2D WidgetPosition, int32 ZOrder)
{
	if (!ShowableClass || !ShowableClass->ImplementsInterface(UShowable::StaticClass()) || !GetWorld())
	{
		UE_LOG(LogTemp, Warning, TEXT("ShowManager: CreateShowableFromClass requires a valid IShowable class and world."));
		return nullptr;
	}
	if (TrackedShowables.Contains(Tag)) RemoveShowable(Tag);

	UObject* Showable = nullptr;
	if (ShowableClass->IsChildOf(UUserWidget::StaticClass()))
	{
		TSubclassOf<UUserWidget> WidgetClass = ShowableClass.Get();
		UUserWidget* Widget = CreateWidget<UUserWidget>(this, WidgetClass);
		if (!Widget) return nullptr;
		if (ShowCanvas)
		{
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ShowCanvas->AddChild(Widget)))
			{
				CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
				CanvasSlot->SetOffsets(FMargin(0.0f));
				CanvasSlot->SetPosition(WidgetPosition);
				CanvasSlot->SetZOrder(ZOrder);
			}
			else
			{
				Widget->RemoveFromParent();
				return nullptr;
			}
		}
		Showable = Widget;
	}
	else if (ShowableClass->IsChildOf(AActor::StaticClass()))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Showable = GetWorld()->SpawnActor<AActor>(Cast<UClass>(ShowableClass), WorldLocation, WorldRotation, SpawnParams);
	}
	else if (ShowableClass->IsChildOf(UActorComponent::StaticClass()))
	{
		AActor* Owner = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), WorldLocation, WorldRotation);
		if (!Owner) return nullptr;
		USceneComponent* Root = NewObject<USceneComponent>(Owner, TEXT("ShowableRoot"));
		Owner->SetRootComponent(Root);
		Owner->AddInstanceComponent(Root);
		Root->RegisterComponent();
		UActorComponent* Component = NewObject<UActorComponent>(Owner, Cast<UClass>(ShowableClass));
		if (!Component || !Component->GetClass()->ImplementsInterface(UShowable::StaticClass()))
		{
			Owner->Destroy();
			return nullptr;
		}
		Owner->AddInstanceComponent(Component);
		Component->RegisterComponent();
		if (UKishiAnimationComponent* AnimationComponent = Cast<UKishiAnimationComponent>(Component))
		{
			AnimationComponent->WorldTarget = Root;
		}
		Showable = Component;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ShowManager: showable class must derive from UUserWidget, AActor, or UActorComponent."));
		return nullptr;
	}

	if (!IsValid(Showable) || !Showable->GetClass()->ImplementsInterface(UShowable::StaticClass())) return nullptr;
	TrackedShowables.Add(Tag, Showable);
	if (FNameProperty* TagProperty = FindFProperty<FNameProperty>(Showable->GetClass(), TEXT("Tag")))
	{
		TagProperty->SetPropertyValue_InContainer(Showable, Tag);
	}
	return Showable;
}

bool UKishiRpyShowManagerWidgetBase::ShowTrackedShowable(FName Tag, const FKishiShowableOptions& Options)
{
	UObject* const* Found = TrackedShowables.Find(Tag);
	if (!Found || !IsValid(*Found) || !(*Found)->GetClass()->ImplementsInterface(UShowable::StaticClass())) return false;
	IShowable::Execute_Show(*Found, Options);
	return true;
}

bool UKishiRpyShowManagerWidgetBase::HideTrackedShowable(FName Tag, const FKishiShowableOptions& Options)
{
	UObject* const* Found = TrackedShowables.Find(Tag);
	if (!Found || !IsValid(*Found) || !(*Found)->GetClass()->ImplementsInterface(UShowable::StaticClass())) return false;
	IShowable::Execute_Hide(*Found, Options);
	return true;
}

bool UKishiRpyShowManagerWidgetBase::RemoveShowable(FName Tag)
{
	UObject** Found = TrackedShowables.Find(Tag);
	if (!Found) return false;
	UObject* Object = *Found;
	TrackedShowables.Remove(Tag);
	if (UUserWidget* Widget = Cast<UUserWidget>(Object))
	{
		Widget->RemoveFromParent();
	}
	else if (AActor* Actor = Cast<AActor>(Object))
	{
		Actor->Destroy();
	}
	else if (UActorComponent* Component = Cast<UActorComponent>(Object))
	{
		if (AActor* Owner = Component->GetOwner()) Owner->Destroy();
		else Component->DestroyComponent();
	}
	return true;
}

TArray<FRpyShowManagerState> UKishiRpyShowManagerWidgetBase::GetCurrentShowStates_Implementation() const
{
	return {ShowState};
}

UUserWidget *UKishiRpyShowManagerWidgetBase::CreateImageWidget(const FRpyImage &Image, const FRpySceneOptions &Options)
{
	if (!ShowCanvas)
		return nullptr;
	const TSubclassOf<UUserWidget> WidgetClass = Image.type == ERpyImageType::ELAYERED_SPRITE ? LayeredSpriteWidgetClass : SpriteWidgetClass;
	if (!WidgetClass)
		return nullptr;
	UUserWidget *Widget = CreateWidget<UUserWidget>(this, WidgetClass);
	if (!Widget)
		return nullptr;
	if (Image.type == ERpyImageType::ESPRITE)
		SetImageOnWidget(Widget, Image.sprite, ShowTargetSize);
	else if (Image.type == ERpyImageType::ELAYERED_SPRITE && Widget->GetClass()->ImplementsInterface(ULayeredSpriteHandler::StaticClass()))
		ILayeredSpriteHandler::Execute_SetSprite(Widget, Image.layeredSprite);
	if (FNameProperty *TagProperty = FindFProperty<FNameProperty>(Widget->GetClass(), TEXT("Tag")))
		TagProperty->SetPropertyValue_InContainer(Widget, Image.tag);
	UCanvasPanelSlot *CanvasSlot = Cast<UCanvasPanelSlot>(ShowCanvas->AddChild(Widget));
	if (!CanvasSlot)
	{
		Widget->RemoveFromParent();
		return nullptr;
	}
	CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	CanvasSlot->SetOffsets(FMargin(0.0f));
	CanvasSlot->SetPosition(FVector2D(Options.position.X, Options.position.Y));
	CanvasSlot->SetZOrder(Options.zorder);
	return Widget;
}

void UKishiRpyShowManagerWidgetBase::RemoveShownWidget(FName Tag)
{
	if (UUserWidget **Widget = ShownWidgets.Find(Tag))
	{
		if (IsValid(*Widget))
			(*Widget)->RemoveFromParent();
		ShownWidgets.Remove(Tag);
	}
}
