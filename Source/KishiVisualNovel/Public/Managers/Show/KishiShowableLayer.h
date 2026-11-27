#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/EngineTypes.h"
#include "Script/RpySceneTypes.h"
#include "Showables/Showable.h"

#include "KishiShowableLayer.generated.h"

class UCanvasPanel;
class URpyInstruction;
class UUserWidget;

/** How a widget showable sits on its layer's canvas. */
UENUM(BlueprintType)
enum class EKishiShowablePlacement : uint8
{
	BottomCenter UMETA(ToolTip = "The widget's bottom center sits at the script position, a fraction of the canvas from its bottom center; the widget is scaled down, never up, to fit the canvas. Used for characters and props."),
	Fill UMETA(ToolTip = "The widget is centered and scaled, up or down, to cover the whole canvas; script positions are ignored. Used for backgrounds."),
};

/**
 * The showables one manager presents on one canvas, tracked by tag: creating them from a showable class, placing and fitting
 * them, playing their show and hide transitions, and the waits the script makes on those transitions. The show manager Base
 * owns one for characters, and the scene manager Base owns one for backgrounds. The owner keeps the script state, resolves the
 * showable class, and calls its own hooks; the layer does the rest.
 */
UCLASS(BlueprintType)
class KISHIVISUALNOVEL_API UKishiShowableLayer : public UObject
{
	GENERATED_BODY()

public:
	/** Owner creates the widgets and implements IRpyScriptInterpreterBaseManager, which completions are reported through. Canvas holds the widget showables; it may be set later. */
	void Initialize(UUserWidget *InOwner, UCanvasPanel *InCanvas);
	void SetCanvas(UCanvasPanel *InCanvas) { Canvas = InCanvas; }
	UCanvasPanel *GetCanvas() const { return Canvas; }
	virtual UWorld *GetWorld() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kishi Showable Layer")
	EKishiShowablePlacement Placement = EKishiShowablePlacement::BottomCenter;
	/** BottomCenter only: scale widget showables down, keeping their bottom center in place, so they never extend past the canvas. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kishi Showable Layer")
	bool bFitToCanvas = true;
	/** Size passed to IShowable::SetShowableAsset; sprite and video showables fit their image to it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kishi Showable Layer")
	FVector2D TargetSize = FVector2D(1920.0f, 1080.0f);
	/** Set while the owner replays saved state, which must not make the script wait. */
	bool bRestoringState = false;

	/**
	 * Shows the image under its tag with ShowableClass: reuses the showable when it can switch in place (the same layered sprite), else creates
	 * it and calls IShowable::SetShowableAsset, then plays the show transition. A hide of the tag that is still playing ends first. Returns Wait
	 * while a transition plays inside an instruction, Continue, or Failed after logging why.
	 */
	ERpyInstructionResult Show(const FRpyImage &Image, const FRpySceneOptions &Options, TSubclassOf<UObject> ShowableClass);
	/** Plays the hide transition on the showable under Tag and removes it when it ends. Returns Wait while it plays inside an instruction, else Continue. */
	ERpyInstructionResult Hide(FName Tag, const FRpySceneOptions &Options);
	/**
	 * Keeps the showable under Tag as it is and removes it after Delay, without a transition, as the old background under a new one.
	 * With ReplacementTag, the delay starts when that showable's show transition does, so a replacement still loading (a video before its first frame) never leaves a gap.
	 */
	void RetireShowable(FName Tag, float Delay, FName ReplacementTag = NAME_None);
	/** Ends every running transition and pending removal at once, and reports the completions the script waits for. */
	void Skip();
	/** Removes every showable and drops the waits without reporting them. */
	void Reset();
	/** Call every frame: refits on a canvas resize, and starts show transitions that waited for the canvas to get a size or for their showable to load. */
	void Tick();

	UObject *CreateShowableFromClass(TSubclassOf<UObject> ShowableClass, FName Tag, FVector WorldLocation, FRotator WorldRotation, FVector2D WidgetPosition, int32 ZOrder);
	UObject *FindShowable(FName Tag) const;
	bool ShowTrackedShowable(FName Tag, const FKishiShowableOptions &Options);
	bool HideTrackedShowable(FName Tag, const FKishiShowableOptions &Options);
	bool RemoveShowable(FName Tag);
	int32 Num() const { return TrackedShowables.Num(); }

private:
	UPROPERTY(Transient)
	UUserWidget *Owner = nullptr;
	UPROPERTY(Transient)
	UCanvasPanel *Canvas = nullptr;
	UPROPERTY(Transient)
	TMap<FName, UObject *> TrackedShowables;
	TMap<FName, FTimerHandle> HideTimers;
	/** Tags whose hide or retirement is still pending; the showable is removed when it ends. */
	TSet<FName> HidingTags;
	/** Canvas size the tracked widgets were last fitted to, so they are refitted when it changes. */
	FVector2D FittedCanvasSize = FVector2D::ZeroVector;
	/** Tags waiting to be refitted once their showable stops animating, since an animation restores the scale it started with. */
	TSet<FName> PendingFits;
	/** The content size each showable was last fitted at, to refit when it changes (a video learns its size when it opens). */
	mutable TMap<TWeakObjectPtr<UObject>, FVector2D> FittedContentSizes;
	/** Show transitions waiting for the canvas to get a size, so widgets shown before its first layout pass start at the fitted scale, or for their showable to load (IShowable::IsLoading). */
	TMap<FName, FRpySceneOptions> PendingShowTransitions;
	/** When each pending show transition began waiting (real time), so a showable that never loads is shown after LoadTimeout. */
	TMap<FName, double> PendingShowStartTimes;
	/** Retirements (tag, delay) that start when the show transition of the replacement under the key starts. */
	TMap<FName, TArray<TPair<FName, float>>> RetiresAfterShow;
	static constexpr float LoadTimeout = 2.0f;

	/** A transition the script waits for: completion is reported for Instruction when it ends. */
	struct FShowableWait
	{
		FName Tag;
		TWeakObjectPtr<URpyInstruction> Instruction;
		FTimerHandle Timer;
	};
	TMap<int32, FShowableWait> Waits;
	int32 NextWaitId = 0;

	FString GetOwnerName() const;
	ERpyInstructionResult BeginWait(FName Tag, float Duration, bool bUseTimer = true);
	void FinishWait(int32 Id);
	void FinishWaitsForTag(FName Tag);
	void FinishHide(FName Tag);
	void ClearHideTimer(FName Tag);
	void ClearWaits();
	/** Starts the show transition that waited under Tag, and the retirements waiting for it. */
	void StartPendingShowTransition(FName Tag);
	FVector2D PlaceWidgetShowable(UObject *Showable, FVector2D Position, int32 ZOrder) const;
	bool IsWidgetShowableAt(UObject *Showable, FVector2D Position) const;
	FVector2D GetOffscreenOffset(UObject *Showable, ERPYTransitionDirection Direction) const;
	void FitWidgetShowable(UObject *Showable) const;
	void PlayShowableTransition(UObject *Showable, const FRpySceneOptions &Options, bool bShow, FVector2D MoveFrom = FVector2D::ZeroVector, bool bEnters = false);
};
