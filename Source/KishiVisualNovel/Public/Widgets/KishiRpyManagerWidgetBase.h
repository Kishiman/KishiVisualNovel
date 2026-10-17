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

#include "KishiRpyManagerWidgetBase.generated.h"

UCLASS(Abstract, Blueprintable)
class KISHIVISUALNOVEL_API UKishiRpyManagerWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetInterpreter(const TScriptInterface<IRpyScriptInterpreter>& InInterpreter) { Interpreter = InInterpreter; }
	virtual TScriptInterface<IRpyScriptInterpreter> GetInterpreter_Implementation() const { return Interpreter; }
	virtual void OnInstructionComplete_Implementation(EInstructionRunTimeType Type);
	virtual void OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset);
	virtual void OnScriptComplete_Implementation();
	virtual void Reset_Implementation();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rpy") TScriptInterface<IRpyScriptInterpreter> Interpreter;
	UFUNCTION(BlueprintImplementableEvent, Category="Rpy") void OnManagerInstructionComplete(EInstructionRunTimeType Type);
	UFUNCTION(BlueprintImplementableEvent, Category="Rpy") void OnManagerViseme(FName Name, EMouthViseme Viseme, float TimeOffset);
	UFUNCTION(BlueprintImplementableEvent, Category="Rpy") void OnManagerScriptComplete();
	UFUNCTION(BlueprintImplementableEvent, Category="Rpy") void OnManagerReset();
};
