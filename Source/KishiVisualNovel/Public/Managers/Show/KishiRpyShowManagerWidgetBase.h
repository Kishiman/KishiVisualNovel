#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Managers/Audio/AudioPlayer.h"
#include "Components/RichTextBlock.h"
#include "Managers/Audio/RpyAudioManager.h"
#include "Managers/Choice/RpyChoiceManager.h"
#include "Managers/Scene/RpySceneManager.h"
#include "Managers/Show/RpyShowManager.h"
#include "Showables/Showable.h"
#include "Managers/Statement/RpyStatementManager.h"
#include "Utils/StringUtils.h"
#include "Player/KishiRpyManagerWidgetBase.h"
#include "Managers/Show/KishiShowableLayer.h"
class UCanvasPanel;

#include "KishiRpyShowManagerWidgetBase.generated.h"

/**
 * Native show manager. Show resolves a showable class for the image's asset (ResolveShowableClass) and hands the image to its
 * showable layer (UKishiShowableLayer), which tracks it by tag, creates it, gives it the asset (IShowable::SetShowableAsset),
 * places it and plays its transitions. Widget showables are placed in the ShowCanvas panel. The Base keeps the show state.
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
	virtual ERpyInstructionResult Show_Implementation(FRpyImage Image, FRpySceneOptions Options) override;
	virtual ERpyInstructionResult Hide_Implementation(FName Tag, FRpySceneOptions Options) override;
	/** Ends every running show and hide transition at once and reports the completions the script waits for. */
	virtual void OnSkipRequested_Implementation() override;

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
	/** Rows checked before the showable registry in Project Settings, so a style can present an asset class with its own widget. Only the widget showable class is used. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kishi Rpy Show Manager Widget Base|Setup")
	TArray<FKishiShowableClassEntry> ShowableClassOverrides;
	/** Size passed to IShowable::SetShowableAsset; sprite and video showables fit their image to it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kishi Rpy Show Manager Widget Base|Setup")
	FVector2D ShowTargetSize = FVector2D(1920.0f, 1080.0f);
	/** The showables on ShowCanvas, tracked by tag. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Kishi Rpy Show Manager Widget Base|State")
	UKishiShowableLayer *ShowableLayer = nullptr;

	/** Optional hook: pick the showable class for an image. The default resolves the image's asset class against ShowableClassOverrides, then the showable registry in Project Settings (URpyShowManagerLibrary::ResolveShowableClass). */
	UFUNCTION(BlueprintNativeEvent, Category = "Kishi Rpy Show Manager Widget Base|Hooks")
	TSubclassOf<UObject> ResolveShowableClass(const FRpyImage &Image) const;
	virtual TSubclassOf<UObject> ResolveShowableClass_Implementation(const FRpyImage &Image) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi Rpy Show Manager Widget Base|Hooks", meta = (ToolTip = "Optional hook called after an image is shown."))
	void OnShowRequested(const FRpyImage &Image, const FRpySceneOptions &Options);
	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi Rpy Show Manager Widget Base|Hooks", meta = (ToolTip = "Optional hook called when an image is hidden."))
	void OnHideRequested(FName Tag, const FRpySceneOptions &Options);

private:
	/** The layer, created on first use and kept in step with the Setup properties. */
	UKishiShowableLayer *GetLayer();
};
