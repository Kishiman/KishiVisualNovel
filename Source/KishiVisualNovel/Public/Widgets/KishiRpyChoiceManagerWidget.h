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
class UPanelWidget;

#include "KishiRpyChoiceManagerWidget.generated.h"

class UKishiChoiceEntryWidget;

UCLASS(Abstract, Blueprintable)
class KISHIVISUALNOVEL_API UKishiRpyChoiceManagerWidget : public UKishiRpyManagerWidgetBase, public IRpyChoiceManager
{
	GENERATED_BODY()
public:
	virtual TScriptInterface<IRpyScriptInterpreter> GetInterpreter_Implementation() const override;
	virtual void OnInstructionComplete_Implementation(EInstructionRunTimeType Type) override;
	virtual void OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset) override;
	virtual void OnScriptComplete_Implementation() override;
	virtual void Reset_Implementation() override;
	virtual FRpyChoiceManagerState GetChoiceManagerState_Implementation() const override { return ChoiceState; }
	virtual void SetChoiceManagerState_Implementation(const FRpyChoiceManagerState& State) override;
	virtual bool Menu_Implementation(const TArray<FString>& Choices) override;
	virtual void Choose_Implementation(int32 Index) override;
	virtual FRpyChoiceManagerState GetCurrentChoiceState_Implementation() const override { return ChoiceState; }
protected:
	UPROPERTY(BlueprintReadOnly, Category="Rpy|Choice") FRpyChoiceManagerState ChoiceState;
	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Rpy|Choice") UPanelWidget* ChoicesPanel = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rpy|Choice") TSubclassOf<UKishiChoiceEntryWidget> ChoiceEntryClass;
	UFUNCTION(BlueprintImplementableEvent, Category="Rpy|Choice") void OnChoicesRequested(const TArray<FString>& Choices);
	UFUNCTION(BlueprintImplementableEvent, Category="Rpy|Choice") void OnChoiceSelected(int32 Index);
};
