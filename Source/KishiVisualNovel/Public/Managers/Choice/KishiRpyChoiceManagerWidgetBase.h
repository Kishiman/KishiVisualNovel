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
class UPanelWidget;

#include "KishiRpyChoiceManagerWidgetBase.generated.h"

class UKishiChoiceEntryWidgetBase;

UCLASS(Abstract, Blueprintable, meta = (DisplayName = "VN Choice Manager Base"))
class KISHIVISUALNOVEL_API UKishiRpyChoiceManagerWidgetBase : public UKishiRpyManagerWidgetBase, public IRpyChoiceManager
{
	GENERATED_BODY()
public:
	virtual void SetInterpreter_Implementation(const TScriptInterface<IRpyScriptInterpreter> &InInterpreter) override { UKishiRpyManagerWidgetBase::SetInterpreter_Implementation(InInterpreter); }
	virtual TScriptInterface<IRpyScriptInterpreter> GetInterpreter_Implementation() const override;
	virtual void OnInstructionComplete_Implementation() override;
	virtual void OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset) override;
	virtual void OnScriptComplete_Implementation() override;
	virtual void Reset_Implementation() override;
	virtual FRpyChoiceManagerState GetChoiceManagerState_Implementation() const override { return ChoiceState; }
	virtual void SetChoiceManagerState_Implementation(const FRpyChoiceManagerState &State) override;
	virtual bool Menu_Implementation(const TArray<FString> &Choices) override;
	virtual void Choose_Implementation(int32 Index) override;
	virtual FRpyChoiceManagerState GetCurrentChoiceState_Implementation() const override { return ChoiceState; }

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Rpy|Choice")
	FRpyChoiceManagerState ChoiceState;
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Rpy|Choice")
	UPanelWidget *ChoicesPanel = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rpy|Choice")
	TSubclassOf<UKishiChoiceEntryWidgetBase> ChoiceEntryClass;
	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi Rpy Choice Manager Widget Base|Hooks")
	void OnChoicesRequested(const TArray<FString> &Choices);
	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi Rpy Choice Manager Widget Base|Hooks")
	void OnChoiceSelected(int32 Index);
	/** Bound to each entry's OnChoiceClicked; dispatches through IRpyChoiceManager::Execute_Choose so Blueprint overrides run. */
	UFUNCTION()
	void HandleChoiceClicked(int32 Index);
};
