#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Audio/AudioPlayer.h"
#include "Components/RichTextBlock.h"
#include "Interfaces/ScriptInterpreterManagers/RpyAudioManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyChoiceManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpySceneManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyShowManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyStatementManager.h"
#include "Utils/StringUtils.h"
#include "Widgets/KishiRpyManagerWidgetBase.h"
class ALevelSequenceActor;
class ULevelSequence;
class ULevelSequencePlayer;
class USceneComponent;

#include "KishiRpySceneManagerWidgetBase.generated.h"

UCLASS(Abstract, Blueprintable, meta = (DisplayName = "VN Scene Manager Base"))
class KISHIVISUALNOVEL_API UKishiRpySceneManagerWidgetBase : public UKishiRpyManagerWidgetBase, public IRpySceneManager
{
	GENERATED_BODY()
public:
	virtual void SetInterpreter_Implementation(const TScriptInterface<IRpyScriptInterpreter> &InInterpreter) override { UKishiRpyManagerWidgetBase::SetInterpreter_Implementation(InInterpreter); }
	virtual TScriptInterface<IRpyScriptInterpreter> GetInterpreter_Implementation() const override;
	virtual void OnInstructionComplete_Implementation() override;
	virtual void OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset) override;
	virtual void OnScriptComplete_Implementation() override;
	virtual void Reset_Implementation() override;
	virtual FRpySceneManagerState GetSceneManagerState_Implementation() const override { return SceneState; }
	virtual void SetSceneManagerState_Implementation(const FRpySceneManagerState &State) override;
	virtual bool Scene_Implementation(FRpyImage Image, FRpySceneOptions Options) override;
	virtual FRpySceneManagerState GetCurrentSceneState_Implementation() const override { return SceneState; }

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	/** Creates SceneSprite from SceneSpriteClass when the tree has none: as the root of an empty tree, behind the other children of a root panel, or else in the viewport behind the player. */
	void EnsureSceneSprite();

	UPROPERTY(BlueprintReadOnly, Category = "Rpy|Scene")
	FRpySceneManagerState SceneState;
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Rpy|Scene")
	UUserWidget *SceneSprite = nullptr;
	bool bOwnsSceneSprite = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rpy|Scene")
	TSubclassOf<UUserWidget> SceneSpriteClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rpy|Scene")
	FVector2D SceneTargetSize = FVector2D(1920.0f, 1080.0f);
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Rpy|Scene")
	ULevelSequencePlayer *ActiveLevelSequencePlayer = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Rpy|Scene")
	ALevelSequenceActor *ActiveLevelSequenceActor = nullptr;

	/** Binding tag that marks the camera track in a scene's level sequence. The binding with this tag is bound to the scene camera so the sequence animates
	 * whichever camera is active. Without the tag, the camera cut track's camera binding is used, or else the sequence's only possessable; a sequence with
	 * none of these plays with the bindings saved in the asset. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rpy|Scene|Camera")
	FName CameraBindingTag = FName(TEXT("Camera"));
	/** Class of the actors that "layer" names. FindSceneAnchor matches an actor of this class whose tags contain the layer name. None matches any actor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rpy|Scene|Camera")
	TSubclassOf<AActor> AnchorClass;
	/** When set, Reset puts the scene camera back on the parent and relative transform it had before the first anchor attach. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rpy|Scene|Camera")
	bool bRestoreCameraOnReset = true;
	/** Camera currently attached to a scene anchor. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Rpy|Scene|Camera")
	AActor *AnchoredCamera = nullptr;
	UPROPERTY(Transient)
	USceneComponent *CameraOriginalParent = nullptr;
	FName CameraOriginalSocket;
	FTransform CameraOriginalRelativeTransform;

	/** Optional override. Returns the camera actor that scene anchors and level sequences move. The default returns the owning player's view target. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Kishi Rpy Scene Manager Widget Base|Hooks")
	AActor *FindSceneCamera() const;
	/** Optional override. Returns the component a scene's "layer" attaches the camera to, or None to leave the camera where it is. The default returns, in order:
	 * the IRpyCameraAnchor component or actor for Layer (URpyCameraAnchorLibrary::FindAnchorByLayerName), then the root of the first AnchorClass actor tagged with
	 * Layer. It returns None for None and "master". */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Kishi Rpy Scene Manager Widget Base|Hooks")
	USceneComponent *FindSceneAnchor(FName Layer) const;

	/** Attaches Camera to Anchor and keeps the camera's relative transform, so later level sequence transforms are relative to the anchor. */
	UFUNCTION(BlueprintCallable, Category = "Kishi Rpy Scene Manager Widget Base")
	bool AttachCameraToAnchor(AActor *Camera, USceneComponent *Anchor);
	/** Stops the active scene level sequence, then plays Sequence once with its camera binding retargeted at Camera. */
	UFUNCTION(BlueprintCallable, Category = "Kishi Rpy Scene Manager Widget Base")
	bool PlaySceneSequence(ULevelSequence *Sequence, AActor *Camera);
	/** Returns the binding to retarget at the scene camera when a sequence has no CameraBindingTag binding: the camera cut track's camera, else the only possessable. */
	static FGuid FindSequenceCameraBinding(class UMovieScene *MovieScene);
	/** Stops and destroys the active scene level sequence. */
	UFUNCTION(BlueprintCallable, Category = "Kishi Rpy Scene Manager Widget Base")
	void StopSceneSequence();
	/** Puts the anchored camera back on the parent and relative transform it had before the first anchor attach. */
	UFUNCTION(BlueprintCallable, Category = "Kishi Rpy Scene Manager Widget Base")
	void RestoreSceneCamera();
	/** Applies the scene's layer anchor and level sequence transition to the scene camera. Returns true when it started a level sequence. */
	bool ApplySceneCameraTransition(const FRpySceneOptions &Options);

	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi Rpy Scene Manager Widget Base|Hooks")
	void OnSceneRequested(const FRpyImage &Image, const FRpySceneOptions &Options);
};
