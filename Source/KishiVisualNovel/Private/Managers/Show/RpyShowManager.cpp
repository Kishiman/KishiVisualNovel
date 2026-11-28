#include "Managers/Show/RpyShowManager.h"

#include "Showables/LayeredSpriteHandler.h"
#include "Showables/LayeredSprite.h"

void URpyShowManagerLibrary::SetShowManagerState_Default(const TScriptInterface<IRpyShowManager> &Target, const FRpyShowManagerState &State)
{
	UObject *Manager = Target.GetObject();
	if (!IsValid(Manager))
	{
		UE_LOG(LogTemp, Error, TEXT("SetShowManagerState_Default requires a valid manager."));
		return;
	}
	for (const FRpyShowImageState &ImageState : State.imageStates)
	{
		// A loaded save cuts to its images instead of replaying their transitions.
		IRpyShowManager::Execute_Show(Manager, ImageState.rpyImage, WithoutTransition(ImageState.options));
	}
}

FRpySceneOptions URpyShowManagerLibrary::WithoutTransition(const FRpySceneOptions &Options)
{
	FRpySceneOptions Result = Options;
	Result.with = ERPYTransitionType::NONE;
	Result.transition = FRpyTransition();
	Result.direction = ERPYTransitionDirection::NONE;
	Result.transitionTime = 0.0f;
	return Result;
}

FKishiShowableOptions URpyShowManagerLibrary::MakeShowableOptions(const FRpySceneOptions &Options)
{
	FKishiShowableOptions Result;
	Result.Attributes = Options.attribute;
	Result.MediaLoop = Options.loop;
	Result.Space = EKishiAnimationSpace::ParentWidget;
	Result.Duration = Options.transitionTime;
	Result.bAnimateOpacity = false;
	switch (Options.with)
	{
	case ERPYTransitionType::FADE:
		Result.Type = EKishiAnimationType::Fade;
		Result.bAnimateOpacity = true;
		break;
	case ERPYTransitionType::BLUR:
		Result.Type = EKishiAnimationType::Blur;
		break;
	case ERPYTransitionType::EASE:
		Result.Type = EKishiAnimationType::Ease;
		break;
	case ERPYTransitionType::MOVE:
		Result.Type = EKishiAnimationType::Move;
		break;
	case ERPYTransitionType::SHAKE:
		Result.Type = EKishiAnimationType::Shake;
		break;
	case ERPYTransitionType::WIPE:
		Result.Type = EKishiAnimationType::Wipe;
		break;
	case ERPYTransitionType::ZOOM:
		Result.Type = EKishiAnimationType::Zoom;
		break;
	default:
		// NONE and LEVEL_SEQUENCE have no showable animation.
		Result.Type = EKishiAnimationType::Linear;
		Result.Duration = 0.0f;
		break;
	}
	return Result;
}

TSubclassOf<UObject> URpyShowManagerLibrary::ResolveShowableClass(const UObject *Asset, EKishiShowableSpace Space, const TArray<FKishiShowableClassEntry> &Overrides)
{
	if (!IsValid(Asset))
		return nullptr;
	if (TSubclassOf<UObject> Override = UKishiVisualNovelSettings::ResolveShowableClass(Overrides, Asset->GetClass(), Space))
		return Override;
	return UKishiVisualNovelSettings::ResolveShowableClass(Asset->GetClass(), Space);
}

bool URpyShowManagerLibrary::CanReuseShowable(UObject *Showable, TSubclassOf<UObject> ShowableClass, const FRpyImage &Image)
{
	// Only a layered sprite switches in place (to new attributes); sprites and videos are recreated so the new image runs its own show transition.
	ULayeredSprite *LayeredSprite = Cast<ULayeredSprite>(Image.asset.Get());
	if (!LayeredSprite || !IsValid(Showable) || !ShowableClass || Showable->GetClass() != ShowableClass.Get())
		return false;
	return Showable->GetClass()->ImplementsInterface(ULayeredSpriteHandler::StaticClass()) &&
		   ILayeredSpriteHandler::Execute_GetSprite(Showable) == LayeredSprite;
}

void URpyShowManagerLibrary::ForwardSpeakerViseme(UObject *Speaker, EMouthViseme Viseme, float TimeOffset)
{
	if (IsValid(Speaker) && Speaker->GetClass()->ImplementsInterface(ULayeredSpriteHandler::StaticClass()))
		ILayeredSpriteHandler::Execute_OnSpeakerViseme(Speaker, Viseme, TimeOffset);
}
