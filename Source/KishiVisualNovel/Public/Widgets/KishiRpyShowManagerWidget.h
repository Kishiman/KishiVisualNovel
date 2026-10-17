#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AudioPlayer.h"
#include "Components/RichTextBlock.h"
#include "Interfaces/ScriptInterpreterManagers/RpyAudioManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyChoiceManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpySceneManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyShowManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyStatementManager.h"
#include "Utils/StringUtils.h"
#include "Widgets/KishiRpyManagerWidgetBase.h"
class UCanvasPanel;

#include "KishiRpyShowManagerWidget.generated.h"

UCLASS(Abstract, Blueprintable)
class KISHIVISUALNOVEL_API UKishiRpyShowManagerWidget : public UKishiRpyManagerWidgetBase, public IRpyShowManager
{
	GENERATED_BODY()
public:
	virtual TScriptInterface<IRpyScriptInterpreter> GetInterpreter_Implementation() const override;
	virtual void OnInstructionComplete_Implementation(EInstructionRunTimeType Type) override;
	virtual void OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset) override;
	virtual void OnScriptComplete_Implementation() override;
	virtual void Reset_Implementation() override;
	virtual FRpyShowManagerState GetShowManagerState_Implementation() const override { return ShowState; }
	virtual void SetShowManagerState_Implementation(const FRpyShowManagerState& State) override;
	virtual bool Show_Implementation(FRpyImage Image, FRpySceneOptions Options) override;
	virtual bool Hide_Implementation(FName Tag, FRpySceneOptions Options) override;
	virtual TArray<FRpyShowManagerState> GetCurrentShowStates_Implementation() const override;
protected:
	UPROPERTY(BlueprintReadOnly, Category="Rpy|Show") FRpyShowManagerState ShowState;
	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Rpy|Show") UCanvasPanel* ShowCanvas = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rpy|Show") TSubclassOf<UUserWidget> SpriteWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rpy|Show") TSubclassOf<UUserWidget> LayeredSpriteWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rpy|Show") FVector2D ShowTargetSize = FVector2D(1920.0f, 1080.0f);
	UPROPERTY(Transient, BlueprintReadOnly, Category="Rpy|Show") TMap<FName, UUserWidget*> ShownWidgets;
	TMap<FName, FTimerHandle> HideTimers;
	UFUNCTION(BlueprintImplementableEvent, Category="Rpy|Show") void OnShowRequested(const FRpyImage& Image, const FRpySceneOptions& Options);
	UFUNCTION(BlueprintImplementableEvent, Category="Rpy|Show") void OnHideRequested(FName Tag, const FRpySceneOptions& Options);
	UUserWidget* CreateImageWidget(const FRpyImage& Image, const FRpySceneOptions& Options);
	void RemoveShownWidget(FName Tag);
};
