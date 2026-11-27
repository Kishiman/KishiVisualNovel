#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Managers/Audio/AudioPlayer.h"
#include "Components/RichTextBlock.h"
#include "Managers/Audio/RpyAudioManager.h"
#include "Managers/Choice/RpyChoiceManager.h"
#include "Managers/Scene/RpySceneManager.h"
#include "Managers/Show/RpyShowManager.h"
#include "Managers/Statement/RpyStatementManager.h"
#include "Utils/StringUtils.h"
#include "Player/KishiRpyManagerWidgetBase.h"
#include "Managers/Show/KishiShowableLayer.h"
class ALevelSequenceActor;
class UCanvasPanel;
class ULevelSequence;
class ULevelSequencePlayer;
class USceneComponent;

#include "KishiRpySceneManagerWidgetBase.generated.h"

/**
 * Native scene manager. A background image goes to the scene's showable layer (UKishiShowableLayer) on SceneCanvas, which fills the
 * canvas with it and plays its transition; the old background stays under the new one until that transition ends. A level sequence
 * image plays on the scene camera, and a scene's layer attaches the camera to a scene anchor.
 */
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
	virtual ERpyInstructionResult Scene_Implementation(FRpyImage Image, FRpySceneOptions Options) override;
	/** Ends a running background transition at once and reports the completion the script waits for. */
	virtual void OnSkipRequested_Implementation() override;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry &MyGeometry, float InDeltaTime) override;
	/**
	 * Without a SceneCanvas in the tree, wraps the tree in a new root canvas: SceneCanvas fills it at the back, and the old root fills it in
	 * front. Called before the Slate widget is built.
	 */
	void EnsureSceneCanvas();

	UPROPERTY(BlueprintReadOnly, Category = "Rpy|Scene")
	FRpySceneManagerState SceneState;
	/** Panel that backgrounds are added to. Name a Canvas Panel "SceneCanvas" in the child Widget Blueprint, or leave it out and one is made behind the rest of the tree. */
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Rpy|Scene")
	UCanvasPanel *SceneCanvas = nullptr;
	/** Legacy: the widget that showed the background before step 7. It is collapsed at start-up; delete it from the Widget Blueprint. */
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Rpy|Scene")
	UWidget *SceneSprite = nullptr;
	/** Rows checked before the showable registry in Project Settings, so a style can present a background asset class with its own widget. Only the widget showable class is used. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rpy|Scene")
	TArray<FKishiShowableClassEntry> ShowableClassOverrides;
	/** Size passed to IShowable::SetShowableAsset; the background is then scaled to cover SceneCanvas. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rpy|Scene")
	FVector2D SceneTargetSize = FVector2D(1920.0f, 1080.0f);
	/** The backgrounds on SceneCanvas: the current one, and the previous one while the current one's transition plays. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Rpy|Scene")
	UKishiShowableLayer *ShowableLayer = nullptr;
	/** Layer tag of the current background, or None. Each background gets a new tag, so the old one can stay under it during the transition. */
	FName BackgroundTag;
	int32 BackgroundCount = 0;
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

	/** Optional override. Returns the camera actor that scene anchors and level sequences move. The default returns the owning player's view target (URpySceneManagerLibrary::FindSceneCamera). */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Kishi Rpy Scene Manager Widget Base|Hooks")
	AActor *FindSceneCamera() const;
	/** Optional override. Returns the component a scene's "layer" attaches the camera to, or None to leave the camera where it is. The default returns, in order:
	 * the IRpyCameraAnchor component or actor for Layer (URpyCameraAnchorLibrary::FindAnchorByLayerName), then the root of the first AnchorClass actor tagged with
	 * Layer. It returns None for None and "master". See URpySceneManagerLibrary::FindSceneAnchor. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Kishi Rpy Scene Manager Widget Base|Hooks")
	USceneComponent *FindSceneAnchor(FName Layer) const;

	/** Attaches Camera to Anchor and keeps the camera's relative transform, so later level sequence transforms are relative to the anchor. */
	UFUNCTION(BlueprintCallable, Category = "Kishi Rpy Scene Manager Widget Base")
	bool AttachCameraToAnchor(AActor *Camera, USceneComponent *Anchor);
	/** Stops the active scene level sequence, then plays Sequence once with its camera binding retargeted at Camera. */
	UFUNCTION(BlueprintCallable, Category = "Kishi Rpy Scene Manager Widget Base")
	bool PlaySceneSequence(ULevelSequence *Sequence, AActor *Camera);
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

private:
	/** The layer, created on first use and kept in step with the Setup properties. */
	UKishiShowableLayer *GetLayer();
};
