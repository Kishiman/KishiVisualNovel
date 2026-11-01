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

	UFUNCTION(BlueprintCallable, Category="Kishi Rpy Show Manager Widget Base|Required", meta=(ToolTip="Required: create and track a widget, actor, or actor component that implements IShowable."))
	UObject* CreateShowableFromClass(TSubclassOf<UObject> ShowableClass, FName Tag, FVector WorldLocation, FRotator WorldRotation, FVector2D WidgetPosition, int32 ZOrder);

	UFUNCTION(BlueprintCallable, Category="Kishi Rpy Show Manager Widget Base|Required", meta=(ToolTip="Required: show a tracked IShowable by tag."))
	bool ShowTrackedShowable(FName Tag, const FKishiShowableOptions& Options);

	UFUNCTION(BlueprintCallable, Category="Kishi Rpy Show Manager Widget Base|Required", meta=(ToolTip="Required: hide a tracked IShowable by tag."))
	bool HideTrackedShowable(FName Tag, const FKishiShowableOptions& Options);

	UFUNCTION(BlueprintCallable, Category="Kishi Rpy Show Manager Widget Base|Required", meta=(ToolTip="Required: remove and release a tracked IShowable by tag."))
	bool RemoveShowable(FName Tag);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Kishi Rpy Show Manager Widget Base|State")
	FRpyShowManagerState ShowState;
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Kishi Rpy Show Manager Widget Base|Setup")
	UCanvasPanel *ShowCanvas = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kishi Rpy Show Manager Widget Base|Setup")
	TSubclassOf<UUserWidget> SpriteWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kishi Rpy Show Manager Widget Base|Setup")
	TSubclassOf<UUserWidget> LayeredSpriteWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kishi Rpy Show Manager Widget Base|Setup")
	FVector2D ShowTargetSize = FVector2D(1920.0f, 1080.0f);
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Kishi Rpy Show Manager Widget Base|State")
	TMap<FName, UUserWidget *> ShownWidgets;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Kishi Rpy Show Manager Widget Base|State")
	TMap<FName, UObject*> TrackedShowables;
	TMap<FName, FTimerHandle> HideTimers;
	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi Rpy Show Manager Widget Base|Hooks", meta=(ToolTip="Optional hook called when an RPY image is requested to show."))
	void OnShowRequested(const FRpyImage &Image, const FRpySceneOptions &Options);
	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi Rpy Show Manager Widget Base|Hooks", meta=(ToolTip="Optional hook called when an RPY image is requested to hide."))
	void OnHideRequested(FName Tag, const FRpySceneOptions &Options);
	UUserWidget *CreateImageWidget(const FRpyImage &Image, const FRpySceneOptions &Options);
	void RemoveShownWidget(FName Tag);
};
