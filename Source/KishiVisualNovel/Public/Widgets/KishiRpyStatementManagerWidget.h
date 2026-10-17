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

#include "KishiRpyStatementManagerWidget.generated.h"

UCLASS(Abstract, Blueprintable)
class KISHIVISUALNOVEL_API UKishiRpyStatementManagerWidget : public UKishiRpyManagerWidgetBase, public IRpyStatementManager
{
	GENERATED_BODY()
public:
	virtual TScriptInterface<IRpyScriptInterpreter> GetInterpreter_Implementation() const override;
	virtual void OnInstructionComplete_Implementation(EInstructionRunTimeType Type) override;
	virtual void OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset) override;
	virtual void OnScriptComplete_Implementation() override;
	virtual void Reset_Implementation() override;
	virtual FRpyStatementManagerState GetStatementManagerState_Implementation() const override { return StatementState; }
	virtual void SetStatementManagerState_Implementation(const FRpyStatementManagerState& State) override;
	virtual bool Say_Implementation(FName Name, const FString& Statement) override;
	virtual FRpyStatementManagerState GetCurrentStatementState_Implementation() const override { return StatementState; }
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	UPROPERTY(BlueprintReadOnly, Category="Rpy|Statement") FRpyStatementManagerState StatementState;
	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Rpy|Statement") URichTextBlock* StatmentText = nullptr;
	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Rpy|Statement") URichTextBlock* NameText = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rpy|Statement", meta=(ClampMin="0.0")) float TextSpeed = 20.0f;
	UPROPERTY(BlueprintReadOnly, Category="Rpy|Statement") int32 StatementCharactersNum = 0;
	UPROPERTY(BlueprintReadOnly, Category="Rpy|Statement") int32 PreviousStatementCharactersNum = 0;
	UPROPERTY(BlueprintReadOnly, Category="Rpy|Statement") FString SpeakerName;
	UPROPERTY(BlueprintReadOnly, Category="Rpy|Statement") FTaggedString TaggedStatement;
	UPROPERTY(BlueprintReadOnly, Category="Rpy|Statement") FMouthViseme LastViseme;
	float CharacterAccumulator = 0.0f;
	UFUNCTION(BlueprintImplementableEvent, Category="Rpy|Statement") void OnStatementRequested(FName Name, const FString& Statement);
	void UpdateVisibleStatement();
	void CompleteOrRevealStatement();
};
