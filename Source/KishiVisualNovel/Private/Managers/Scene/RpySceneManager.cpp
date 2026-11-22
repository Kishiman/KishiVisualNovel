#include "Managers/Scene/RpySceneManager.h"

#include "Camera/PlayerCameraManager.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "MovieScene.h"
#include "MovieSceneTrack.h"
#include "Sections/MovieSceneCameraCutSection.h"
#include "World/RpyCameraAnchor.h"

// Scene camera tracing. Filter the Output Log on "[VNScene]".
#define VNSCENE_LIB_LOG(Format, ...) UE_LOG(LogTemp, Log, TEXT("[VNScene] " Format), ##__VA_ARGS__)

void URpySceneManagerLibrary::SetSceneManagerState_Default(const TScriptInterface<IRpySceneManager> &Target, const FRpySceneManagerState &State)
{
	UObject *Manager = Target.GetObject();
	if (!IsValid(Manager))
	{
		UE_LOG(LogTemp, Error, TEXT("SetSceneManagerState_Default requires a valid manager."));
		return;
	}
	IRpySceneManager::Execute_Scene(Manager, State.rpyImage, State.options);
}

AActor *URpySceneManagerLibrary::FindSceneCamera(const UObject *WorldContextObject, APlayerController *PlayerController)
{
	if (!PlayerController)
		PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	if (!PlayerController)
		return nullptr;
	if (PlayerController->PlayerCameraManager)
		if (AActor *ViewTarget = PlayerController->PlayerCameraManager->GetViewTarget())
			return ViewTarget;
	return PlayerController->GetViewTarget();
}

USceneComponent *URpySceneManagerLibrary::FindSceneAnchor(const UObject *WorldContextObject, FName Layer, TSubclassOf<AActor> AnchorClass)
{
	static const FName MasterLayer(TEXT("master"));
	UWorld *World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) : nullptr;
	if (Layer.IsNone() || Layer == MasterLayer || !World)
		return nullptr;
	const TScriptInterface<IRpyCameraAnchor> CameraAnchor = URpyCameraAnchorLibrary::FindAnchorByLayerName(World, Layer);
	if (UObject *AnchorObject = CameraAnchor.GetObject())
	{
		if (USceneComponent *AnchorComponent = Cast<USceneComponent>(AnchorObject))
			return AnchorComponent;
		if (AActor *AnchorActor = IRpyCameraAnchor::Execute_GetActor(AnchorObject))
			if (USceneComponent *AnchorRoot = AnchorActor->GetRootComponent())
				return AnchorRoot;
	}
	UClass *Class = AnchorClass ? AnchorClass.Get() : AActor::StaticClass();
	for (TActorIterator<AActor> It(World, Class); It; ++It)
		if (It->ActorHasTag(Layer) && It->GetRootComponent())
			return It->GetRootComponent();
	UE_LOG(LogTemp, Warning, TEXT("%s: no camera anchor or %s actor tagged with scene layer '%s'; the camera stays on its current parent."), *GetNameSafe(WorldContextObject),
		   *Class->GetName(), *Layer.ToString());
	return nullptr;
}

bool URpySceneManagerLibrary::AttachCameraToAnchor(AActor *Camera, USceneComponent *Anchor)
{
	if (!IsValid(Camera) || !IsValid(Anchor) || Anchor->GetOwner() == Camera)
		return false;
	USceneComponent *CameraRoot = Camera->GetRootComponent();
	if (!CameraRoot)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot attach camera %s to anchor %s because the camera has no root component."), *Camera->GetName(), *Anchor->GetName());
		return false;
	}
	if (CameraRoot->Mobility != EComponentMobility::Movable)
		CameraRoot->SetMobility(EComponentMobility::Movable);
	const bool bAttached = CameraRoot->AttachToComponent(Anchor, FAttachmentTransformRules::KeepRelativeTransform);
	VNSCENE_LIB_LOG("attached camera %s to anchor %s.%s: %d", *Camera->GetName(), *GetNameSafe(Anchor->GetOwner()), *Anchor->GetName(), bAttached ? 1 : 0);
	return bAttached;
}

bool URpySceneManagerLibrary::PlayCameraSequence(UObject *WorldContextObject, ULevelSequence *Sequence, AActor *Camera, FName CameraBindingTag, ULevelSequencePlayer *&OutPlayer,
												 ALevelSequenceActor *&OutActor)
{
	OutPlayer = nullptr;
	OutActor = nullptr;
	if (!IsValid(Sequence))
		return false;
	FMovieSceneSequencePlaybackSettings Settings;
	// No autoplay: the camera binding must be set before the first evaluation.
	Settings.bAutoPlay = false;
	ALevelSequenceActor *SequenceActor = nullptr;
	ULevelSequencePlayer *Player = ULevelSequencePlayer::CreateLevelSequencePlayer(WorldContextObject, Sequence, Settings, SequenceActor);
	if (!Player || !SequenceActor)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: could not create a player for level sequence %s."), *GetNameSafe(WorldContextObject), *Sequence->GetName());
		return false;
	}
	OutPlayer = Player;
	OutActor = SequenceActor;
	UMovieScene *MovieScene = Sequence->GetMovieScene();
	if (IsValid(Camera) && MovieScene)
	{
		if (!CameraBindingTag.IsNone() && MovieScene->AllTaggedBindings().Contains(CameraBindingTag))
		{
			SequenceActor->SetBindingByTag(CameraBindingTag, {Camera});
			VNSCENE_LIB_LOG("bound sequence %s tag '%s' to camera %s", *Sequence->GetName(), *CameraBindingTag.ToString(), *Camera->GetName());
		}
		else
		{
			const FGuid CameraGuid = FindSequenceCameraBinding(MovieScene);
			if (CameraGuid.IsValid())
			{
				SequenceActor->SetBinding(UE::MovieScene::FFixedObjectBindingID(CameraGuid, MovieSceneSequenceID::Root), {Camera});
				VNSCENE_LIB_LOG("bound sequence %s camera binding '%s' to camera %s (no binding tagged '%s')", *Sequence->GetName(),
								*MovieScene->GetObjectDisplayName(CameraGuid).ToString(), *Camera->GetName(), *CameraBindingTag.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("%s: level sequence %s has no binding tagged '%s', no camera cut binding and more than one possessable; it plays with the bindings saved in the asset, so the scene camera does not move."),
					   *GetNameSafe(WorldContextObject), *Sequence->GetName(), *CameraBindingTag.ToString());
			}
		}
	}
	Player->Play();
	return true;
}

void URpySceneManagerLibrary::StopCameraSequence(ULevelSequencePlayer *Player, ALevelSequenceActor *Actor)
{
	if (IsValid(Player))
		Player->Stop();
	if (IsValid(Actor))
		Actor->Destroy();
}

ULevelSequence *URpySceneManagerLibrary::GetTransitionSequence(const FRpySceneOptions &Options)
{
	if (Options.with != ERPYTransitionType::LEVEL_SEQUENCE && Options.transition.type != ERPYTransitionType::LEVEL_SEQUENCE)
		return nullptr;
	ULevelSequence *Sequence = Options.transition.levelSequence;
	if (!IsValid(Sequence) && !Options.transition.path.IsEmpty())
		Sequence = LoadObject<ULevelSequence>(nullptr, *Options.transition.path);
	if (!IsValid(Sequence))
		UE_LOG(LogTemp, Error, TEXT("Scene transition '%s' has no level sequence (path '%s')."), *Options.transition.name.ToString(), *Options.transition.path);
	return Sequence;
}

FGuid URpySceneManagerLibrary::FindSequenceCameraBinding(UMovieScene *MovieScene)
{
	if (!MovieScene)
		return FGuid();
	// The camera a camera cut cuts to is the sequence's camera.
	if (const UMovieSceneTrack *CameraCutTrack = MovieScene->GetCameraCutTrack())
		for (const UMovieSceneSection *Section : CameraCutTrack->GetAllSections())
			if (const UMovieSceneCameraCutSection *CutSection = Cast<UMovieSceneCameraCutSection>(Section))
			{
				const FGuid Guid = CutSection->GetCameraBindingID().GetGuid();
				if (Guid.IsValid() && MovieScene->FindPossessable(Guid))
					return Guid;
			}
	// Otherwise a sequence that possesses a single object animates the camera.
	if (MovieScene->GetPossessableCount() == 1)
		return MovieScene->GetPossessable(0).GetGuid();
	return FGuid();
}

#undef VNSCENE_LIB_LOG
