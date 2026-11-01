#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Audio/AudioPlayer.h"
#include "Components/RichTextBlock.h"
#include "Interfaces/ScriptInterpreterManagers/RpyAudioManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyChoiceManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpySceneManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyShowManager.h"
#include "Interfaces/Showable.h"
#include "Interfaces/ScriptInterpreterManagers/RpyStatementManager.h"
#include "Utils/StringUtils.h"
#include "Widgets/KishiRpyManagerWidgetBase.h"
class UCanvasPanel;

#include "KishiRpyShowManagerWidgetBase.generated.h"

/**
 * Native show manager. Every shown image is an IShowable tracked by its tag: Show resolves a showable class
 * for the image (ResolveShowableClass), creates it (CreateShowableFromClass), gives it the image's asset
 * (ApplyImageToShowable) and calls IShowable::Show. Widget showables are placed in the ShowCanvas panel.
 */
UCLASS(Abstract, Blueprintable, meta = (DisplayName = "VN Show Manager Base"))
class KISHIVISUALNOVEL_API UKishiRpyShowManagerWidgetBase : public UKishiRpyManagerWidgetBase, public IRpyShowManager
{
	GENERATED_BODY()
public:
	virtual void SetInterpreter_Implementation(const TScriptInterface<IRpyScriptInterpreter> &InInterpreter) override { UKishiRpyManagerWidgetBase::SetInterpreter_Implementation(InInterpreter); }
	virtual TScriptInterface<IRpyScriptInterpreter> GetInterpreter_Implementation() const override;
	virtual void OnInstructionComplete_Implementation() override;
	virtual void OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset) override;
	virtual void OnScriptComplete_Implementation() override;
	virtual void Reset_Implementation() override;
	virtual FRpyShowManagerState GetShowManagerState_Implementation() const override { return ShowState; }
	virtual void SetShowManagerState_Implementation(const FRpyShowManagerState &State) override;
	virtual bool Show_Implementation(FRpyImage Image, FRpySceneOptions Options) override;
	virtual bool Hide_Implementation(FName Tag, FRpySceneOptions Options) override;
	virtual TArray<FRpyShowManagerState> GetCurrentShowStates_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category = "Kishi Rpy Show Manager Widget Base|Showables", meta = (ToolTip = "Create an IShowable widget, actor or actor component and track it under Tag, replacing any showable already tracked there. Widgets are sized to their content and their bottom center is placed at WidgetPosition, a fraction of ShowCanvas from its bottom center (0.4 moves it 40% right), like script positions; actors and components are spawned at WorldLocation. Logs an error and returns None on failure."))
	UObject *CreateShowableFromClass(TSubclassOf<UObject> ShowableClass, FName Tag, FVector WorldLocation, FRotator WorldRotation, FVector2D WidgetPosition, int32 ZOrder);

	UFUNCTION(BlueprintPure, Category = "Kishi Rpy Show Manager Widget Base|Showables", meta = (ToolTip = "Return the showable tracked under Tag, or None."))
	UObject *FindShowable(FName Tag) const;

	UFUNCTION(BlueprintCallable, Category = "Kishi Rpy Show Manager Widget Base|Showables", meta = (ToolTip = "Call IShowable::Show on the showable tracked under Tag. Returns false if there is none."))
	bool ShowTrackedShowable(FName Tag, const FKishiShowableOptions &Options);

	UFUNCTION(BlueprintCallable, Category = "Kishi Rpy Show Manager Widget Base|Showables", meta = (ToolTip = "Call IShowable::Hide on the showable tracked under Tag, without removing it. Returns false if there is none."))
	bool HideTrackedShowable(FName Tag, const FKishiShowableOptions &Options);

	UFUNCTION(BlueprintCallable, Category = "Kishi Rpy Show Manager Widget Base|Showables", meta = (ToolTip = "Stop tracking the showable under Tag and remove it: widgets from their parent, actors and component owners from the world."))
	bool RemoveShowable(FName Tag);

	/** Converts script scene options into the options IShowable::Show and IShowable::Hide take. */
	UFUNCTION(BlueprintPure, Category = "Kishi Rpy Show Manager Widget Base|Showables")
	static FKishiShowableOptions MakeShowableOptions(const FRpySceneOptions &Options);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry &MyGeometry, float InDeltaTime) override;

	/** Scale widget showables down, keeping their bottom center in place, so they never extend past ShowCanvas. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kishi Rpy Show Manager Widget Base|Setup")
	bool bFitShowablesToCanvas = true;
	UPROPERTY(BlueprintReadOnly, Category = "Kishi Rpy Show Manager Widget Base|State")
	FRpyShowManagerState ShowState;
	/** Panel that widget showables are added to. Name a Canvas Panel "ShowCanvas" in the child Widget Blueprint. */
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Kishi Rpy Show Manager Widget Base|Setup")
	UCanvasPanel *ShowCanvas = nullptr;
	/** Showable created for sprite and video images by the default ResolveShowableClass, such as KishiSpriteWidget. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kishi Rpy Show Manager Widget Base|Setup", meta = (MustImplement = "Showable"))
	TSubclassOf<UObject> SpriteWidgetClass;
	/** Showable created for layered sprite images by the default ResolveShowableClass, such as KishiLayeredSpriteWidget. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kishi Rpy Show Manager Widget Base|Setup", meta = (MustImplement = "Showable"))
	TSubclassOf<UObject> LayeredSpriteWidgetClass;
	/** Size that sprite and video images are fitted to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kishi Rpy Show Manager Widget Base|Setup")
	FVector2D ShowTargetSize = FVector2D(1920.0f, 1080.0f);
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Kishi Rpy Show Manager Widget Base|State")
	TMap<FName, UObject *> TrackedShowables;
	TMap<FName, FTimerHandle> HideTimers;

	/** Optional hook: pick the showable class for an image. The default returns LayeredSpriteWidgetClass for layered sprites and SpriteWidgetClass for sprites and videos, and None for other types. Override to support other image types or world-space showables. */
	UFUNCTION(BlueprintNativeEvent, Category = "Kishi Rpy Show Manager Widget Base|Hooks")
	TSubclassOf<UObject> ResolveShowableClass(const FRpyImage &Image) const;
	virtual TSubclassOf<UObject> ResolveShowableClass_Implementation(const FRpyImage &Image) const;

	/** Optional hook: give a new showable the image's asset. The default calls ILayeredSpriteHandler::SetSprite for layered sprites, and a SetSprite or SetFileMediaSource function for sprites and videos. Return false if the asset could not be applied. */
	UFUNCTION(BlueprintNativeEvent, Category = "Kishi Rpy Show Manager Widget Base|Hooks")
	bool ApplyImageToShowable(UObject *Showable, const FRpyImage &Image);
	virtual bool ApplyImageToShowable_Implementation(UObject *Showable, const FRpyImage &Image);

	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi Rpy Show Manager Widget Base|Hooks", meta = (ToolTip = "Optional hook called after an image is shown."))
	void OnShowRequested(const FRpyImage &Image, const FRpySceneOptions &Options);
	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi Rpy Show Manager Widget Base|Hooks", meta = (ToolTip = "Optional hook called when an image is hidden."))
	void OnHideRequested(FName Tag, const FRpySceneOptions &Options);

private:
	/** True when the showable under the image's tag can switch to it in place: same class and, for layered sprites, the same asset. */
	bool CanReuseShowable(UObject *Showable, TSubclassOf<UObject> ShowableClass, const FRpyImage &Image) const;
	/** Places a widget showable's bottom center at Position, a fraction of ShowCanvas from its bottom center. Returns the offset in pixels from the new position back to the old one. */
	FVector2D PlaceWidgetShowable(UObject *Showable, FVector2D Position, int32 ZOrder) const;
	/** Sets a widget showable's render scale so its content fits ShowCanvas, scaling down only. */
	void FitWidgetShowable(UObject *Showable) const;
	/** ShowCanvas size the tracked widgets were last fitted to, so they are refitted when it changes. */
	FVector2D FittedCanvasSize = FVector2D::ZeroVector;
	/** Tags waiting to be refitted once their showable stops animating, since an animation restores the scale it started with. */
	TSet<FName> PendingFits;
	/** Show transitions waiting for ShowCanvas to get a size, so widgets shown before its first layout pass start at the fitted scale. */
	TMap<FName, FRpySceneOptions> PendingShowTransitions;
	/** True when a widget showable's bottom center is already pinned at Position, or it is not in a canvas slot. */
	bool IsWidgetShowableAt(UObject *Showable, FVector2D Position) const;
	/** Render offset that puts a widget showable just past the canvas edge in Direction, or zero for NONE. */
	FVector2D GetOffscreenOffset(UObject *Showable, ERPYTransitionDirection Direction) const;
	/** bEnters: the showable was not on screen, so a directional move or ease slides it in from the canvas edge. */
	void PlayShowableTransition(UObject *Showable, const FRpySceneOptions &Options, bool bShow, FVector2D MoveFrom = FVector2D::ZeroVector, bool bEnters = false);
	void ClearHideTimer(FName Tag);
	void RemoveAllShowables();
};
