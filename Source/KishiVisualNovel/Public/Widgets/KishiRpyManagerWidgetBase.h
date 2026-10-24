#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Audio/AudioPlayer.h"
#include "Components/RichTextBlock.h"
#include "Interfaces/ScriptInterpreterManagers/RpyAudioManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyChoiceManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpySceneManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyShowManager.h"
#include "Interfaces/ScriptInterpreterManagers/RpyStatementManager.h"
#include "Utils/StringUtils.h"

#include "KishiRpyManagerWidgetBase.generated.h"

/**
 * Shared native behavior for manager widgets. Derive a concrete manager widget from the
 * appropriate specialized Base class, then implement its Blueprint hooks under that base class's category.
 */
UCLASS(Abstract, Blueprintable, meta = (DisplayName = "VN Manager Base"))
class KISHIVISUALNOVEL_API UKishiRpyManagerWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void SetInterpreter_Implementation(const TScriptInterface<IRpyScriptInterpreter>& InInterpreter) { Interpreter = InInterpreter; }
	virtual TScriptInterface<IRpyScriptInterpreter> GetInterpreter_Implementation() const { return Interpreter; }
	virtual void OnInstructionComplete_Implementation();
	virtual void OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset);
	virtual void OnScriptComplete_Implementation();
	virtual void Reset_Implementation();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rpy") TScriptInterface<IRpyScriptInterpreter> Interpreter;
	/** Child Blueprint hook called when the interpreter reports a completed instruction. */
	UFUNCTION(BlueprintImplementableEvent, Category="Kishi Rpy Manager Widget Base|Hooks") void OnManagerInstructionComplete();
	/** Child Blueprint hook called when the dialogue viseme changes. */
	UFUNCTION(BlueprintImplementableEvent, Category="Kishi Rpy Manager Widget Base|Hooks") void OnManagerViseme(FName Name, EMouthViseme Viseme, float TimeOffset);
	/** Child Blueprint hook called when the script completes. */
	UFUNCTION(BlueprintImplementableEvent, Category="Kishi Rpy Manager Widget Base|Hooks") void OnManagerScriptComplete();
	/** Child Blueprint hook called when the manager resets. */
	UFUNCTION(BlueprintImplementableEvent, Category="Kishi Rpy Manager Widget Base|Hooks") void OnManagerReset();
};
