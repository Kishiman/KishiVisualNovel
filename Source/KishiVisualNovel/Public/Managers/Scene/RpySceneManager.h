#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Player/RpyScriptInterpreterBaseManager.h"
#include "Script/RpyScriptTypes.h"

#include "RpySceneManager.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API URpySceneManager : public URpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpySceneManager : public IRpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
public:
  // get state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpySceneManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  FRpySceneManagerState GetSceneManagerState() const;
  virtual FRpySceneManagerState GetSceneManagerState_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpySceneManager::GetSceneManagerState")); return {}; }

  // set state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpySceneManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  void SetSceneManagerState(const FRpySceneManagerState &State);
  virtual void SetSceneManagerState_Implementation(const FRpySceneManagerState &State) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpySceneManager::SetSceneManagerState")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpySceneManager|Required", meta = (ToolTip = "Required: replace the background with this image; an image of type ENONE (scene _) clears it. Return Continue once it is shown, Wait if you will report completion through the interpreter yourself, or Failed, after logging why, if it cannot be shown."))
  ERpyInstructionResult Scene(FRpyImage rpyImage, FRpySceneOptions options);
  virtual ERpyInstructionResult Scene_Implementation(FRpyImage rpyImage, FRpySceneOptions options) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpySceneManager::Scene")); return ERpyInstructionResult::Failed; }
};

class AActor;
class ALevelSequenceActor;
class APlayerController;
class ULevelSequence;
class ULevelSequencePlayer;
class UMovieScene;
class USceneComponent;

/** Shared logic for IRpySceneManager implementers: the widget Base calls these, and a component or Blueprint manager can call them the same way. None of them touch widgets. */
UCLASS()
class KISHIVISUALNOVEL_API URpySceneManagerLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable, Category = "Rpy Scene Manager Library", meta = (DefaultToSelf = "Target", ToolTip = "Default SetSceneManagerState: call Scene on the manager with the saved image and options."))
  static void SetSceneManagerState_Default(const TScriptInterface<IRpySceneManager> &Target, const FRpySceneManagerState &State);

  UFUNCTION(BlueprintCallable, Category = "Rpy Scene Manager Library", meta = (WorldContext = "WorldContextObject", ToolTip = "Return the camera actor that scene anchors and level sequences move: the view target of PlayerController, or of the first player when it is None."))
  static AActor *FindSceneCamera(const UObject *WorldContextObject, APlayerController *PlayerController);

  UFUNCTION(BlueprintCallable, Category = "Rpy Scene Manager Library", meta = (WorldContext = "WorldContextObject", ToolTip = "Return the component a scene's layer attaches the camera to: the IRpyCameraAnchor component or actor for Layer, then the root of the first AnchorClass actor (any actor when None) tagged with Layer. Returns None for None and 'master', and logs a warning when nothing matches."))
  static USceneComponent *FindSceneAnchor(const UObject *WorldContextObject, FName Layer, TSubclassOf<AActor> AnchorClass);

  UFUNCTION(BlueprintCallable, Category = "Rpy Scene Manager Library", meta = (ToolTip = "Attach Camera's root to Anchor, keeping its relative transform so later level sequence transforms are relative to the anchor. Makes the root movable. Returns false if either is None, the anchor belongs to the camera, or the camera has no root."))
  static bool AttachCameraToAnchor(AActor *Camera, USceneComponent *Anchor);

  UFUNCTION(BlueprintCallable, Category = "Rpy Scene Manager Library", meta = (WorldContext = "WorldContextObject", ToolTip = "Create a player for Sequence, retarget its camera binding at Camera, and play it once. The camera binding is the one tagged CameraBindingTag, else the camera cut track's camera, else the only possessable. Returns false, after logging why, if no player could be created. Stop the returned player and actor with StopCameraSequence when the scene changes."))
  static bool PlayCameraSequence(UObject *WorldContextObject, ULevelSequence *Sequence, AActor *Camera, FName CameraBindingTag, ULevelSequencePlayer *&OutPlayer, ALevelSequenceActor *&OutActor);

  UFUNCTION(BlueprintCallable, Category = "Rpy Scene Manager Library", meta = (ToolTip = "Stop the player and destroy the actor that PlayCameraSequence returned. Either may be None."))
  static void StopCameraSequence(ULevelSequencePlayer *Player, ALevelSequenceActor *Actor);

  UFUNCTION(BlueprintCallable, Category = "Rpy Scene Manager Library", meta = (ToolTip = "Return the level sequence a scene transition plays, loading it from the transition's path when needed, or None when the options have no level sequence transition. Logs an error when the transition asks for a sequence it does not have."))
  static ULevelSequence *GetTransitionSequence(const FRpySceneOptions &Options);

  /** The binding to retarget at the scene camera when a sequence has no tagged binding: the camera cut track's camera, else the only possessable. */
  static FGuid FindSequenceCameraBinding(UMovieScene *MovieScene);
};
