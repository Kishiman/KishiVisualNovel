#include "Widgets/KishiRpyShowManagerWidget.h"
#include "Interfaces/RpyScriptInterpreterBaseManager.h"
#include "Interfaces/RpyScriptInterpreter.h"
#include "Interfaces/LayeredSpriteHandler.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
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

void UKishiRpyShowManagerWidget::SetShowManagerState_Implementation(const FRpyShowManagerState& State)
{
	for (const TPair<FName, UUserWidget*>& Pair : ShownWidgets)
	{
		if (IsValid(Pair.Value)) Pair.Value->RemoveFromParent();
	}
	ShownWidgets.Reset();
	ShowState = State;
	for (const FRpyShowImageState& ImageState : ShowState.imageStates)
	{
		Show_Implementation(ImageState.rpyImage, ImageState.options);
	}
}

bool UKishiRpyShowManagerWidget::Show_Implementation(FRpyImage Image, FRpySceneOptions Options)
{
	FRpyShowImageState* Existing = ShowState.imageStates.FindByPredicate([&Image](const FRpyShowImageState& Item)
	{
		return Item.name == Image.tag;
	});
	if (!Existing)
	{
		Existing = &ShowState.imageStates.AddDefaulted_GetRef();
	}
	Existing->name = Image.tag;
	Existing->rpyImage = Image;
	Existing->options = Options;
	if (ShowCanvas)
	{
		if (FTimerHandle* HideTimer = HideTimers.Find(Image.tag))
		{
			if (GetWorld()) GetWorld()->GetTimerManager().ClearTimer(*HideTimer);
			HideTimers.Remove(Image.tag);
		}
		RemoveShownWidget(Image.tag);
		if (UUserWidget* ImageWidget = CreateImageWidget(Image, Options))
		{
			ShownWidgets.Add(Image.tag, ImageWidget);
			PlayImageTransition(ImageWidget, Options, 0);
		}
	}
	OnShowRequested(Image, Options);
	return Image.type == ERpyImageType::ESPRITE || Image.type == ERpyImageType::ELAYERED_SPRITE;
}

bool UKishiRpyShowManagerWidget::Hide_Implementation(FName Tag, FRpySceneOptions Options)
{
	const int32 Index = ShowState.imageStates.IndexOfByPredicate([Tag](const FRpyShowImageState& Item)
	{
		return Item.name == Tag || Item.rpyImage.tag == Tag;
	});
	if (Index == INDEX_NONE)
	{
		return false;
	}
	OnHideRequested(Tag, Options);
	const FName StateTag = ShowState.imageStates[Index].name;
	if (Options.with != ERPYTransitionType::NONE && Options.transitionTime > 0.0f)
	{
		UUserWidget** Widget = ShownWidgets.Find(StateTag);
		if (Widget && IsValid(*Widget) && GetWorld())
		{
			PlayImageTransition(*Widget, Options, 1);
			FTimerHandle& Handle = HideTimers.FindOrAdd(StateTag);
			GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateWeakLambda(this, [this, StateTag]() { HideTimers.Remove(StateTag); RemoveShownWidget(StateTag); }), Options.transitionTime, false);
		}
		else RemoveShownWidget(StateTag);
	}
	else RemoveShownWidget(StateTag);
	ShowState.imageStates.RemoveAt(Index);
	return true;
}

void UKishiRpyShowManagerWidget::Reset_Implementation()
{
	for (const TPair<FName, UUserWidget*>& Pair : ShownWidgets)
	{
		if (IsValid(Pair.Value)) Pair.Value->RemoveFromParent();
	}
	ShownWidgets.Reset();
	if (GetWorld())
	{
		for (TPair<FName, FTimerHandle>& Pair : HideTimers) GetWorld()->GetTimerManager().ClearTimer(Pair.Value);
	}
	HideTimers.Reset();
	ShowState.imageStates.Reset();
	UKishiRpyManagerWidgetBase::Reset_Implementation();
}

TArray<FRpyShowManagerState> UKishiRpyShowManagerWidget::GetCurrentShowStates_Implementation() const
{
	return { ShowState };
}

UUserWidget* UKishiRpyShowManagerWidget::CreateImageWidget(const FRpyImage& Image, const FRpySceneOptions& Options)
{
	if (!ShowCanvas) return nullptr;
	const TSubclassOf<UUserWidget> WidgetClass = Image.type == ERpyImageType::ELAYERED_SPRITE ? LayeredSpriteWidgetClass : SpriteWidgetClass;
	if (!WidgetClass) return nullptr;
	UUserWidget* Widget = CreateWidget<UUserWidget>(this, WidgetClass);
	if (!Widget) return nullptr;
	if (Image.type == ERpyImageType::ESPRITE) SetImageOnWidget(Widget, Image.sprite, ShowTargetSize);
	else if (Image.type == ERpyImageType::ELAYERED_SPRITE && Widget->GetClass()->ImplementsInterface(ULayeredSpriteHandler::StaticClass()))
		ILayeredSpriteHandler::Execute_SetSprite(Widget, Image.layeredSprite);
	if (FNameProperty* TagProperty = FindFProperty<FNameProperty>(Widget->GetClass(), TEXT("Tag")))
		TagProperty->SetPropertyValue_InContainer(Widget, Image.tag);
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ShowCanvas->AddChild(Widget));
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

void UKishiRpyShowManagerWidget::RemoveShownWidget(FName Tag)
{
	if (UUserWidget** Widget = ShownWidgets.Find(Tag))
	{
		if (IsValid(*Widget)) (*Widget)->RemoveFromParent();
		ShownWidgets.Remove(Tag);
	}
}
