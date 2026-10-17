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
class ULevelSequencePlayer;

#include "KishiRpySceneManagerWidget.generated.h"

UCLASS(Abstract, Blueprintable)
class KISHIVISUALNOVEL_API UKishiRpySceneManagerWidget : public UKishiRpyManagerWidgetBase, public IRpySceneManager
{
	GENERATED_BODY()
public:
	virtual TScriptInterface<IRpyScriptInterpreter> GetInterpreter_Implementation() const override;
	virtual void OnInstructionComplete_Implementation(EInstructionRunTimeType Type) override;
	virtual void OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset) override;
	virtual void OnScriptComplete_Implementation() override;
	virtual void Reset_Implementation() override;
	virtual FRpySceneManagerState GetSceneManagerState_Implementation() const override { return SceneState; }
	virtual void SetSceneManagerState_Implementation(const FRpySceneManagerState& State) override;
	virtual bool Scene_Implementation(FRpyImage Image, FRpySceneOptions Options) override;
	virtual FRpySceneManagerState GetCurrentSceneState_Implementation() const override { return SceneState; }
protected:
	UPROPERTY(BlueprintReadOnly, Category="Rpy|Scene") FRpySceneManagerState SceneState;
	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Rpy|Scene") UUserWidget* SceneSprite = nullptr;
	bool bOwnsSceneSprite = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rpy|Scene") TSubclassOf<UUserWidget> SceneSpriteClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rpy|Scene") FVector2D SceneTargetSize = FVector2D(1920.0f, 1080.0f);
	UPROPERTY(Transient, BlueprintReadOnly, Category="Rpy|Scene") ULevelSequencePlayer* ActiveLevelSequencePlayer = nullptr;
	UFUNCTION(BlueprintImplementableEvent, Category="Rpy|Scene") void OnSceneRequested(const FRpyImage& Image, const FRpySceneOptions& Options);
};
