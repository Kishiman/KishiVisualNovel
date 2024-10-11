// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "Animation/WidgetAnimation.h"
#include "MovieScene.h"
#include "MovieSceneSequence.h"
#include "MovieScenePossessable.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MovieScene.h"
#include "MovieSceneSection.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "AnimationSolverVector2D.h"

#include "WidgetAnimationLibrary.generated.h"

/**
 *
 */
UCLASS()
class KISHIVISUALNOVEL_API UWidgetAnimationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// float
	UFUNCTION(BlueprintCallable, category = Widget)
	static UWidgetAnimation *CloneAnimation(UUserWidget *Target, UWidgetAnimation *Animation)
	{
		if (!Animation || !Target)
		{
			return nullptr;
		}

		// Create a new animation instance (deep copy concept)
		UWidgetAnimation *NewAnimation = NewObject<UWidgetAnimation>(Target);

		if (NewAnimation && Animation->MovieScene)
		{
			// Duplicate the MovieScene data
			NewAnimation->MovieScene = DuplicateObject(Animation->MovieScene, NewAnimation);

			// Copy over all bindings from the source animation
			NewAnimation->AnimationBindings = Animation->AnimationBindings;

			// At this point, you can modify the bindings or make any changes
		}

		return NewAnimation;
	}

	UFUNCTION(BlueprintCallable, category = Widget)
	static UWidgetAnimation *BindAnimationByName(UUserWidget *Target, UWidgetAnimation *Animation, FString BindingName, UWidget *NewWidget)
	{
		if (!Target || !Animation || !NewWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid Target, Animation, or NewWidget provided."));
			return nullptr;
		}
		auto clonedAnimation = Animation;

		// Create a new GUID for the binding
		FGuid NewWidgetGuid = FGuid::NewGuid();
		// Get the MovieScene from the animation
		UMovieScene *MovieScene = clonedAnimation->MovieScene;
		if (!MovieScene)
		{
			UE_LOG(LogTemp, Warning, TEXT("MovieScene is null in the provided Animation."));
			return nullptr;
		}

		UE_LOG(LogTemp, Log, TEXT("MovieScene->GetBindings().Num(): %d"), MovieScene->GetBindings().Num());

		// Iterate over all object bindings in the MovieScene
		for (FMovieSceneBinding &Binding : (TArray<FMovieSceneBinding> &)MovieScene->GetBindings())
		{
			UE_LOG(LogTemp, Log, TEXT("Binding.GetName(): %s"), *Binding.GetName());
			UE_LOG(LogTemp, Log, TEXT("Binding.GetName(): %s"), *Binding.GetObjectGuid().ToString());
			// Check if the binding's name matches the given binding name
			if (Binding.GetName() == BindingName)
			{
				Binding.SetObjectGuid(NewWidgetGuid);
				Binding.SetName(NewWidget->GetName());
				// We also need to change the track's owners to be the MovieScene.
				for (UMovieSceneTrack *Track : Binding.GetTracks())
				{
					Track->Modify();
				}
				UE_LOG(LogTemp, Log, TEXT("Successfully replaced binding for widget: %s"), *NewWidget->GetName());
				return clonedAnimation; // Return the  clonedAnimation
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("Could not find binding with the name: %s"), *BindingName);
		return nullptr;
	}
};
