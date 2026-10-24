#include "Widgets/KishiLayeredSpriteWidgetBase.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "Animation/Transitions/RpyMoveTransition.h"
#include "Engine/World.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "PaperSprite.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"

void UKishiLayeredSpriteWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	LoadLayers();
}

void UKishiLayeredSpriteWidgetBase::NativeTick(const FGeometry &MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	SuperTick(InDeltaTime);
}

ULayeredSprite *UKishiLayeredSpriteWidgetBase::GetSprite_Implementation() const
{
	return LayeredSprite;
}

void UKishiLayeredSpriteWidgetBase::SetSprite_Implementation(ULayeredSprite *Sprite)
{
	SetLayeredSprite(Sprite);
}

bool UKishiLayeredSpriteWidgetBase::GetLayerDisplay_Implementation(FName LayerName) const
{
	const bool *bDisplayed = LayerDisplay.Find(LayerName);
	return bDisplayed ? *bDisplayed : false;
}

void UKishiLayeredSpriteWidgetBase::SetLayerDisplay_Implementation(FName LayerName, bool bValue)
{
	SetLayerDisplayInternal(LayerName, bValue, FRpySceneOptions(), 0.0f, false);
}

void UKishiLayeredSpriteWidgetBase::SetLayeredSprite(ULayeredSprite *Sprite)
{
	if (LayeredSprite == Sprite)
	{
		return;
	}

	LayeredSprite = Sprite;
	LayerDisplay.Reset();
	if (LayeredSprite)
	{
		for (const FSpriteLayer &Layer : LayeredSprite->layers)
		{
			LayerDisplay.Add(Layer.name, Layer.displayed);
		}
	}
	LoadLayers();
}

void UKishiLayeredSpriteWidgetBase::LoadLayers()
{
	ClearLayers();
	if (!IsValid(LayeredSprite) || !IsValid(Canvas))
	{
		return;
	}

	for (int32 Index = 0; Index < LayeredSprite->layers.Num(); ++Index)
	{
		const FSpriteLayer &Layer = LayeredSprite->layers[Index];
		const bool *bExistingDisplay = LayerDisplay.Find(Layer.name);
		if (!bExistingDisplay)
		{
			LayerDisplay.Add(Layer.name, Layer.displayed);
		}

		UUserWidget *LayerWidget = AddLayer(Layer.name, Layer.sprite, Index, Layer.offset);
		if (LayerWidget)
		{
			const bool *bDisplayed = LayerDisplay.Find(Layer.name);
			const bool bShouldDisplay = bDisplayed ? *bDisplayed : Layer.displayed;
			LayerWidget->SetRenderOpacity(bShouldDisplay ? 1.0f : 0.0f);
			LayerWidget->SetVisibility(bShouldDisplay ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		}
	}
}

void UKishiLayeredSpriteWidgetBase::ClearLayers()
{
	for (TPair<FName, UUserWidget *> &Layer : Layers)
	{
		if (IsValid(Layer.Value))
		{
			Layer.Value->RemoveFromParent();
		}
	}
	Layers.Reset();
}

UUserWidget *UKishiLayeredSpriteWidgetBase::AddLayer(FName Name, UPaperSprite *Sprite, int32 ZOrder, FVector2D Position)
{
	if (UUserWidget **Existing = Layers.Find(Name))
	{
		return *Existing;
	}
	if (!IsValid(Canvas) || !LayerWidgetClass)
	{
		return nullptr;
	}

	UUserWidget *LayerWidget = CreateWidget<UUserWidget>(this, LayerWidgetClass);
	if (!LayerWidget)
	{
		return nullptr;
	}

	InitializeLayerWidget(LayerWidget, Name, Sprite, MatchSize);
	UCanvasPanelSlot *CanvasSlot = Cast<UCanvasPanelSlot>(Canvas->AddChild(LayerWidget));
	if (!CanvasSlot)
	{
		LayerWidget->RemoveFromParent();
		return nullptr;
	}

	CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	CanvasSlot->SetOffsets(FMargin(0.0f));
	CanvasSlot->SetPosition(Position);
	CanvasSlot->SetZOrder(ZOrder);
	LayerWidget->SetRenderOpacity(0.0f);
	LayerWidget->SetVisibility(ESlateVisibility::Collapsed);
	Layers.Add(Name, LayerWidget);
	return LayerWidget;
}

void UKishiLayeredSpriteWidgetBase::ShowLayer(FName LayerName, const FRpySceneOptions &SceneOptions, float Delay)
{
	SetLayerDisplayInternal(LayerName, true, SceneOptions, Delay, true);
}

void UKishiLayeredSpriteWidgetBase::HideLayer(FName LayerName, const FRpySceneOptions &SceneOptions, float Delay)
{
	SetLayerDisplayInternal(LayerName, false, SceneOptions, Delay, true);
}

void UKishiLayeredSpriteWidgetBase::SetLayerDisplayInternal(FName LayerName, bool bValue, const FRpySceneOptions &SceneOptions, float Delay, bool bPlayTransition)
{
	LayerDisplay.Add(LayerName, bValue);
	UUserWidget **LayerWidget = Layers.Find(LayerName);
	if (LayerWidget && IsValid(*LayerWidget))
	{
		(*LayerWidget)->SetVisibility(bValue || bPlayTransition ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		const bool bTransitionStarted = bPlayTransition && PlayKishiWidgetTransition(*LayerWidget, bValue, SceneOptions, Delay);
		if (!bTransitionStarted)
		{
			(*LayerWidget)->SetRenderOpacity(bValue ? 1.0f : 0.0f);
		}
		OnLayerDisplayChanged(*LayerWidget, bValue, SceneOptions, Delay);
	}
}

bool UKishiLayeredSpriteWidgetBase::PlayKishiWidgetTransition(UUserWidget *LayerWidget, bool bShow, const FRpySceneOptions &SceneOptions, float Delay) const
{
	if (!IsValid(LayerWidget))
	{
		return false;
	}

	UFunction *PlayTransitionFunction = LayerWidget->FindFunction(FName(TEXT("PlayTransition")));
	if (!PlayTransitionFunction)
	{
		return false;
	}

	FStructOnScope Parameters(PlayTransitionFunction);
	if (FStructProperty *SceneOptionsProperty = FindFProperty<FStructProperty>(PlayTransitionFunction, TEXT("sceneOptions")))
	{
		if (SceneOptionsProperty->Struct == FRpySceneOptions::StaticStruct())
		{
			SceneOptionsProperty->CopyCompleteValue_InContainer(Parameters.GetStructMemory(), &SceneOptions);
		}
	}
	if (FProperty *PlayModeProperty = FindFProperty<FProperty>(PlayTransitionFunction, TEXT("PlayMode")))
	{
		if (FEnumProperty *EnumProperty = CastField<FEnumProperty>(PlayModeProperty))
		{
			EnumProperty->GetUnderlyingProperty()->SetIntPropertyValue(EnumProperty->ContainerPtrToValuePtr<void>(Parameters.GetStructMemory()), static_cast<int64>(bShow ? 0 : 1));
		}
		else if (FNumericProperty *NumericProperty = CastField<FNumericProperty>(PlayModeProperty))
		{
			NumericProperty->SetIntPropertyValue(NumericProperty->ContainerPtrToValuePtr<void>(Parameters.GetStructMemory()), static_cast<int64>(bShow ? 0 : 1));
		}
	}
	if (FFloatProperty *DelayProperty = FindFProperty<FFloatProperty>(PlayTransitionFunction, TEXT("Delay")))
	{
		DelayProperty->SetPropertyValue_InContainer(Parameters.GetStructMemory(), Delay);
	}
	LayerWidget->ProcessEvent(PlayTransitionFunction, Parameters.GetStructMemory());
	return true;
}

void UKishiLayeredSpriteWidgetBase::DisplayLayersByAttribute(const FString &Attribute, TArray<FName> &NamesToShow, TArray<FName> &NamesToHide)
{
	NamesToShow.Reset();
	NamesToHide.Reset();
	if (!IsValid(LayeredSprite))
	{
		return;
	}

	TArray<FName> MatchingNames;
	TArray<FName> InvertedNames;
	LayeredSprite->FindNamesByAttribute(Attribute, MatchingNames, InvertedNames);
	for (const FSpriteLayer &Layer : LayeredSprite->layers)
	{
		const bool bDisplayed = GetLayerDisplay_Implementation(Layer.name);
		if (MatchingNames.Contains(Layer.name) && !bDisplayed)
		{
			NamesToShow.Add(Layer.name);
		}
		else if (InvertedNames.Contains(Layer.name) && bDisplayed)
		{
			NamesToHide.Add(Layer.name);
		}
	}
}

void UKishiLayeredSpriteWidgetBase::UpdateVisemeLayer(EMouthViseme Viseme, float TimeOffset)
{
	if (!IsValid(LayeredSprite))
	{
		return;
	}

	TArray<FSpriteLayer> VisemeLayers;
	LayeredSprite->FindLayersByGroup(LayeredSprite->visemeGroupName, VisemeLayers);
	if (VisemeLayers.Num() == 0)
	{
		return;
	}

	for (const FSpriteLayer &Layer : VisemeLayers)
	{
		if (GetLayerDisplay_Implementation(Layer.name))
		{
			NeutralVisemeLayer = Layer.name;
			break;
		}
	}

	if (Viseme == EMouthViseme::Neutral && TimeOffset >= VisemeNeutralOffset)
	{
		ResetNeutralVisemeLayer();
		return;
	}

	UPaperSprite *const *VisemeSprite = LayeredSprite->visemeMap.Find(Viseme);
	UUserWidget **LayerWidget = Layers.Find(NeutralVisemeLayer);
	if (VisemeSprite && IsValid(*VisemeSprite) && LayerWidget && IsValid(*LayerWidget))
	{
		SetLayerWidgetSprite(*LayerWidget, *VisemeSprite);
	}
}

void UKishiLayeredSpriteWidgetBase::ResetNeutralVisemeLayer()
{
	if (!IsValid(LayeredSprite))
	{
		return;
	}

	TArray<FSpriteLayer> VisemeLayers;
	LayeredSprite->FindLayersByGroup(LayeredSprite->visemeGroupName, VisemeLayers);
	for (const FSpriteLayer &Layer : VisemeLayers)
	{
		UUserWidget **LayerWidget = Layers.Find(Layer.name);
		if (LayerWidget && IsValid(*LayerWidget))
		{
			SetLayerWidgetSprite(*LayerWidget, Layer.sprite);
		}
	}
}

void UKishiLayeredSpriteWidgetBase::TransitionLayers(const FRpySceneOptions &SceneOptions)
{
	TArray<FName> NamesToShow;
	TArray<FName> NamesToHide;
	DisplayLayersByAttribute(SceneOptions.attribute, NamesToShow, NamesToHide);
	if (SceneOptions.with == ERPYTransitionType::NONE)
	{
		HideShowTransition(SceneOptions, NamesToShow, NamesToHide);
	}
	else
	{
		SyncTransition(SceneOptions, NamesToShow, NamesToHide);
	}
}

void UKishiLayeredSpriteWidgetBase::HideShowTransition(const FRpySceneOptions &SceneOptions, const TArray<FName> &NamesToShow, const TArray<FName> &NamesToHide)
{
	for (const FName &Name : NamesToHide)
	{
		HideLayer(Name, SceneOptions);
	}
	for (const FName &Name : NamesToShow)
	{
		ShowLayer(Name, SceneOptions);
	}
}

void UKishiLayeredSpriteWidgetBase::SyncTransition(const FRpySceneOptions &SceneOptions, const TArray<FName> &NamesToShow, const TArray<FName> &NamesToHide)
{
	FRpySceneOptions LayerOptions = SceneOptions;
	if (SceneOptions.with == ERPYTransitionType::MOVE || SceneOptions.with == ERPYTransitionType::EASE)
	{
		LayerOptions = PlayMove(SceneOptions);
	}
	HideShowTransition(LayerOptions, NamesToShow, NamesToHide);
}

void UKishiLayeredSpriteWidgetBase::StartMove(FVector2D MoveTo, float Speed, EInterpolationMethod Method)
{
	const FVector2D Start = RenderTransform.Translation;
	const float Duration = Speed == 0.0f ? 100.0f : 1.0f / Speed;
	UClass *TransitionClass = nullptr;
	switch (Method)
	{
	case EInterpolationMethod::Constant:
		TransitionClass = URpyMoveTransition::StaticClass();
		break;
	case EInterpolationMethod::Ease:
		TransitionClass = URpyMoveEaseTransition::StaticClass();
		break;
	default:
		ActiveTranslateTransition = nullptr;
		break;
	}

	if (TransitionClass)
	{
		ActiveTranslateTransition = NewObject<URpyTransitionBase>(this, TransitionClass);
		ActiveTranslateTransition->Start2D(this, Start, MoveTo, Duration);
	}
}

void UKishiLayeredSpriteWidgetBase::SuperTick(float DeltaSeconds)
{
	if (IsValid(ActiveTranslateTransition) && ActiveTranslateTransition->TickTransition(DeltaSeconds))
	{
		ActiveTranslateTransition = nullptr;
	}
}

FRpySceneOptions UKishiLayeredSpriteWidgetBase::PlayMove(const FRpySceneOptions &SceneOptions)
{
	FRpySceneOptions Result = SceneOptions;
	EInterpolationMethod Method = EInterpolationMethod::None;
	switch (SceneOptions.with)
	{
	case ERPYTransitionType::EASE:
		Method = EInterpolationMethod::Ease;
		Result.with = ERPYTransitionType::NONE;
		break;
	case ERPYTransitionType::BLUR:
	case ERPYTransitionType::DISSOLVE:
	case ERPYTransitionType::FADE:
	case ERPYTransitionType::MOVE:
	case ERPYTransitionType::SHAKE:
	case ERPYTransitionType::WIPE:
	case ERPYTransitionType::ZOOM:
		Method = EInterpolationMethod::Constant;
		if (SceneOptions.with == ERPYTransitionType::MOVE)
		{
			Result.with = ERPYTransitionType::NONE;
		}
		break;
	case ERPYTransitionType::NONE:
	case ERPYTransitionType::LEVEL_SEQUENCE:
	default:
		break;
	}

	const FVector2D LocalPosition(SceneOptions.position.X, SceneOptions.position.Y);
	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
	StartMove(LocalPosition * ViewportSize, SceneOptions.transitionTime == 0.0f ? 100.0f : 1.0f / SceneOptions.transitionTime, Method);
	Result.position = FVector::ZeroVector;
	return Result;
}

UPanelWidget *UKishiLayeredSpriteWidgetBase::GetPanelWidget() const
{
	return Cast<UPanelWidget>(GetParent());
}

void UKishiLayeredSpriteWidgetBase::InitializeLayerWidget_Implementation(UUserWidget *LayerWidget, FName LayerName, UPaperSprite *Sprite, FVector2D TargetSize)
{
	if (!IsValid(LayerWidget))
	{
		return;
	}
	if (FNameProperty *NameProperty = FindFProperty<FNameProperty>(LayerWidget->GetClass(), TEXT("Name")))
	{
		NameProperty->SetPropertyValue_InContainer(LayerWidget, LayerName);
	}
	SetLayerWidgetSprite(LayerWidget, Sprite);
	if (FStructProperty *SizeProperty = FindFProperty<FStructProperty>(LayerWidget->GetClass(), TEXT("MaxSize")))
	{
		if (SizeProperty->Struct == TBaseStructure<FVector2D>::Get())
		{
			SizeProperty->CopyCompleteValue_InContainer(LayerWidget, &TargetSize);
		}
	}
}

void UKishiLayeredSpriteWidgetBase::SetLayerWidgetSprite(UUserWidget *LayerWidget, UPaperSprite *Sprite) const
{
	if (!IsValid(LayerWidget))
	{
		return;
	}
	UFunction *SetSpriteFunction = LayerWidget->FindFunction(FName(TEXT("SetSprite")));
	if (!SetSpriteFunction)
	{
		SetSpriteFunction = LayerWidget->FindFunction(FName(TEXT("Set Sprite")));
	}
	if (!SetSpriteFunction)
	{
		return;
	}

	FStructOnScope Parameters(SetSpriteFunction);
	if (FObjectPropertyBase *SpriteProperty = FindFProperty<FObjectPropertyBase>(SetSpriteFunction, TEXT("Sprite")))
	{
		SpriteProperty->SetObjectPropertyValue_InContainer(Parameters.GetStructMemory(), Sprite);
	}
	const FVector2D TargetSize = MatchSize;
	if (FStructProperty *SizeProperty = FindFProperty<FStructProperty>(SetSpriteFunction, TEXT("TargetSize")))
	{
		if (SizeProperty->Struct == TBaseStructure<FVector2D>::Get())
		{
			SizeProperty->CopyCompleteValue_InContainer(Parameters.GetStructMemory(), &TargetSize);
		}
	}
	LayerWidget->ProcessEvent(SetSpriteFunction, Parameters.GetStructMemory());
}
