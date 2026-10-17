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

#include "KishiRpyAudioManagerWidget.generated.h"

UCLASS(Abstract, Blueprintable)
class KISHIVISUALNOVEL_API UKishiRpyAudioManagerWidget : public UKishiRpyManagerWidgetBase, public IRpyAudioManager
{
	GENERATED_BODY()
public:
	virtual TScriptInterface<IRpyScriptInterpreter> GetInterpreter_Implementation() const override;
	virtual void OnInstructionComplete_Implementation(EInstructionRunTimeType Type) override;
	virtual void OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset) override;
	virtual void OnScriptComplete_Implementation() override;
	virtual void Reset_Implementation() override;
	virtual FRpyAudioManagerState GetAudioManagerState_Implementation() const override { return AudioState; }
	virtual void SetAudioManagerState_Implementation(const FRpyAudioManagerState& State) override;
	virtual bool PauseAudio_Implementation(float Timeout) override;
	virtual bool StopAudio_Implementation(FName Channel, FRpyAudioOptions Options) override;
	virtual bool QueueAudio_Implementation(FName Channel, FRpyAudio Audio, FRpyAudioOptions Options) override;
	virtual bool PlayAudio_Implementation(FName Channel, FRpyAudio Audio, FRpyAudioOptions Options) override;
	virtual TArray<FRpyAudioManagerState> GetCurrentAudioStates_Implementation() const override;
protected:
	virtual void NativeOnInitialized() override;
	UPROPERTY(BlueprintReadOnly, Category="Rpy|Audio") FRpyAudioManagerState AudioState;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Rpy|Audio") UAudioPlayer* AudioPlayer = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rpy|Audio") TMap<FName, USoundClass*> AudioChannels;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rpy|Audio") USoundWave* Blip = nullptr;
	UFUNCTION(BlueprintImplementableEvent, Category="Rpy|Audio") void OnPauseAudioRequested(float Timeout);
	UFUNCTION(BlueprintImplementableEvent, Category="Rpy|Audio") void OnStopAudioRequested(FName Channel, const FRpyAudioOptions& Options);
	UFUNCTION(BlueprintImplementableEvent, Category="Rpy|Audio") void OnQueueAudioRequested(FName Channel, const FRpyAudio& Audio, const FRpyAudioOptions& Options);
	UFUNCTION(BlueprintImplementableEvent, Category="Rpy|Audio") void OnPlayAudioRequested(FName Channel, const FRpyAudio& Audio, const FRpyAudioOptions& Options);
	USoundWave* ResolveAudio(const FRpyAudio& Audio) const;
};
