#include "Managers/Show/RpyShowManager.h"

#include "Player/KishiRpyWidgetReflection.h"
#include "Showables/LayeredSpriteHandler.h"
#include "Showables/VideoMediaSprite.h"

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
		IRpyShowManager::Execute_Show(Manager, ImageState.rpyImage, ImageState.options);
	}
}

FKishiShowableOptions URpyShowManagerLibrary::MakeShowableOptions(const FRpySceneOptions &Options)
{
	FKishiShowableOptions Result;
	Result.Attributes = Options.attribute;
	Result.Space = EKishiAnimationSpace::ParentWidget;
	Result.Duration = Options.transitionTime;
	Result.bAnimateOpacity = false;
	switch (Options.with)
	{
	case ERPYTransitionType::FADE:
		Result.Type = EKishiAnimationType::Fade;
		Result.bAnimateOpacity = true;
		break;
	case ERPYTransitionType::DISSOLVE:
		Result.Type = EKishiAnimationType::Dissolve;
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

bool URpyShowManagerLibrary::ApplyImageToShowable(UObject *Showable, const FRpyImage &Image, FVector2D TargetSize)
{
	if (!IsValid(Showable))
		return false;
	switch (Image.type)
	{
	case ERpyImageType::ELAYERED_SPRITE:
		if (!IsValid(Image.layeredSprite) || !Showable->GetClass()->ImplementsInterface(ULayeredSpriteHandler::StaticClass()))
			return false;
		ILayeredSpriteHandler::Execute_SetSprite(Showable, Image.layeredSprite);
		return true;
	case ERpyImageType::ESPRITE:
		return IsValid(Image.sprite) && SetImageOnWidget(Cast<UUserWidget>(Showable), Image.sprite, TargetSize);
	case ERpyImageType::EVIDEO_MEDIA_SPRITE:
		return IsValid(Image.videoMediaSprite) && IsValid(Image.videoMediaSprite->fileMediaSource) &&
			   SetMediaOnWidget(Cast<UUserWidget>(Showable), Image.videoMediaSprite->fileMediaSource, TargetSize);
	default:
		return false;
	}
}

bool URpyShowManagerLibrary::CanReuseShowable(UObject *Showable, TSubclassOf<UObject> ShowableClass, const FRpyImage &Image)
{
	// Sprites and videos are recreated so the new image runs its own show transition.
	if (!IsValid(Showable) || !ShowableClass || Showable->GetClass() != ShowableClass.Get() || Image.type != ERpyImageType::ELAYERED_SPRITE)
		return false;
	return Showable->GetClass()->ImplementsInterface(ULayeredSpriteHandler::StaticClass()) &&
		   ILayeredSpriteHandler::Execute_GetSprite(Showable) == Image.layeredSprite;
}

void URpyShowManagerLibrary::ForwardSpeakerViseme(UObject *Speaker, EMouthViseme Viseme, float TimeOffset)
{
	if (IsValid(Speaker) && Speaker->GetClass()->ImplementsInterface(ULayeredSpriteHandler::StaticClass()))
		ILayeredSpriteHandler::Execute_OnSpeakerViseme(Speaker, Viseme, TimeOffset);
}
