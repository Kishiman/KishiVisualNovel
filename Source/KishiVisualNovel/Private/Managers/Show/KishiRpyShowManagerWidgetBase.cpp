#include "Managers/Show/KishiRpyShowManagerWidgetBase.h"
#include "Player/RpyScriptInterpreterBaseManager.h"
#include "Player/RpyScriptInterpreter.h"
#include "Components/CanvasPanel.h"
#include "UObject/UnrealType.h"

// Show manager tracing. Filter the Output Log on "[VNShow]".
#define VNSHOW_LOG(Format, ...) UE_LOG(LogTemp, Log, TEXT("[VNShow] %s: " Format), *GetName(), ##__VA_ARGS__)

namespace
{
	FString DescribeOptions(const FRpySceneOptions &Options)
	{
		return FString::Printf(TEXT("attribute='%s' with=%s time=%.2f positionType=%s positionName='%s' position=%s zorder=%d"),
							   *Options.attribute, *UEnum::GetValueAsString(Options.with), Options.transitionTime,
							   *UEnum::GetValueAsString(Options.positionType), *Options.positionName.ToString(),
							   *Options.position.ToString(), Options.zorder);
	}
}

UKishiShowableLayer *UKishiRpyShowManagerWidgetBase::GetLayer()
{
	if (!ShowableLayer)
	{
		ShowableLayer = NewObject<UKishiShowableLayer>(this, TEXT("ShowLayer"));
		ShowableLayer->Initialize(this, ShowCanvas);
	}
	ShowableLayer->SetCanvas(ShowCanvas);
	ShowableLayer->Placement = EKishiShowablePlacement::BottomCenter;
	ShowableLayer->bFitToCanvas = bFitShowablesToCanvas;
	ShowableLayer->TargetSize = ShowTargetSize;
	return ShowableLayer;
}

void UKishiRpyShowManagerWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (IsDesignTime())
		return;
	GetLayer();
	VNSHOW_LOG("initialized. class=%s ShowCanvas=%s ShowableClassOverrides=%d ShowTargetSize=%s",
			   *GetClass()->GetName(), *GetNameSafe(ShowCanvas), ShowableClassOverrides.Num(), *ShowTargetSize.ToString());
	for (const FName FunctionName : {FName(TEXT("Show")), FName(TEXT("Hide")), FName(TEXT("SetShowManagerState")), FName(TEXT("Reset"))})
	{
		const UFunction *Function = GetClass()->FindFunctionByName(FunctionName);
		if (Function && Function->GetOuterUClass() != UKishiRpyShowManagerWidgetBase::StaticClass() && !Function->GetOuterUClass()->HasAnyClassFlags(CLASS_Interface | CLASS_Native))
			UE_LOG(LogTemp, Warning, TEXT("[VNShow] %s: the Blueprint %s overrides %s; the C++ implementation will not run unless it calls the parent node."),
				   *GetName(), *Function->GetOuterUClass()->GetName(), *FunctionName.ToString());
	}
}

void UKishiRpyShowManagerWidgetBase::NativeTick(const FGeometry &MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (ShowableLayer)
		ShowableLayer->Tick();
}

void UKishiRpyShowManagerWidgetBase::SetShowManagerState_Implementation(const FRpyShowManagerState &State)
{
	VNSHOW_LOG("SetShowManagerState with %d images.", State.imageStates.Num());
	const FRpyShowManagerState Saved = State;
	UKishiShowableLayer *ShowLayer = GetLayer();
	ShowLayer->Reset();
	ShowState.imageStates.Reset();
	TGuardValue<bool> Restoring(ShowLayer->bRestoringState, true);
	URpyShowManagerLibrary::SetShowManagerState_Default(this, Saved);
}

ERpyInstructionResult UKishiRpyShowManagerWidgetBase::Show_Implementation(FRpyImage Image, FRpySceneOptions Options)
{
	VNSHOW_LOG("Show tag='%s' asset=%s | %s", *Image.tag.ToString(), *Image.asset.ToString(), *DescribeOptions(Options));
	FRpyShowImageState *Existing = ShowState.imageStates.FindByPredicate([&Image](const FRpyShowImageState &Item)
																		 { return Item.name == Image.tag; });
	if (!Existing)
		Existing = &ShowState.imageStates.AddDefaulted_GetRef();
	Existing->name = Image.tag;
	Existing->rpyImage = Image;
	Existing->options = Options;
	const ERpyInstructionResult Result = GetLayer()->Show(Image, Options, ResolveShowableClass(Image));
	OnShowRequested(Image, Options);
	return Result;
}

ERpyInstructionResult UKishiRpyShowManagerWidgetBase::Hide_Implementation(FName Tag, FRpySceneOptions Options)
{
	VNSHOW_LOG("Hide '%s' | %s", *Tag.ToString(), *DescribeOptions(Options));
	const int32 Index = ShowState.imageStates.IndexOfByPredicate([Tag](const FRpyShowImageState &Item)
																 { return Item.name == Tag || Item.rpyImage.tag == Tag; });
	if (Index == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: hide '%s' ignored, no image with that tag is shown."), *GetName(), *Tag.ToString());
		return ERpyInstructionResult::Continue;
	}
	OnHideRequested(Tag, Options);
	const FName StateTag = ShowState.imageStates[Index].name;
	ShowState.imageStates.RemoveAt(Index);
	return GetLayer()->Hide(StateTag, Options);
}

void UKishiRpyShowManagerWidgetBase::OnSkipRequested_Implementation()
{
	if (ShowableLayer)
		ShowableLayer->Skip();
}

void UKishiRpyShowManagerWidgetBase::Reset_Implementation()
{
	VNSHOW_LOG("Reset, removing %d showables.", ShowableLayer ? ShowableLayer->Num() : 0);
	if (ShowableLayer)
		ShowableLayer->Reset();
	ShowState.imageStates.Reset();
	UKishiRpyManagerWidgetBase::Reset_Implementation();
}

void UKishiRpyShowManagerWidgetBase::OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset)
{
	UKishiRpyManagerWidgetBase::OnDialogueViseme_Implementation(Name, Viseme, TimeOffset);
	URpyShowManagerLibrary::ForwardSpeakerViseme(FindShowable(Name), Viseme, TimeOffset);
}

UObject *UKishiRpyShowManagerWidgetBase::CreateShowableFromClass(TSubclassOf<UObject> ShowableClass, FName Tag, FVector WorldLocation, FRotator WorldRotation, FVector2D WidgetPosition, int32 ZOrder)
{
	return GetLayer()->CreateShowableFromClass(ShowableClass, Tag, WorldLocation, WorldRotation, WidgetPosition, ZOrder);
}

UObject *UKishiRpyShowManagerWidgetBase::FindShowable(FName Tag) const
{
	return ShowableLayer ? ShowableLayer->FindShowable(Tag) : nullptr;
}

bool UKishiRpyShowManagerWidgetBase::ShowTrackedShowable(FName Tag, const FKishiShowableOptions &Options)
{
	return GetLayer()->ShowTrackedShowable(Tag, Options);
}

bool UKishiRpyShowManagerWidgetBase::HideTrackedShowable(FName Tag, const FKishiShowableOptions &Options)
{
	return GetLayer()->HideTrackedShowable(Tag, Options);
}

bool UKishiRpyShowManagerWidgetBase::RemoveShowable(FName Tag)
{
	return GetLayer()->RemoveShowable(Tag);
}

TSubclassOf<UObject> UKishiRpyShowManagerWidgetBase::ResolveShowableClass_Implementation(const FRpyImage &Image) const
{
	return URpyShowManagerLibrary::ResolveShowableClass(Image.LoadAsset(), EKishiShowableSpace::Widget, ShowableClassOverrides);
}

#undef VNSHOW_LOG
