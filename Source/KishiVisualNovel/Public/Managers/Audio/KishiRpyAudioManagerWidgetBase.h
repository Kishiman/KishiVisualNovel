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

#include "KishiRpyAudioManagerWidgetBase.generated.h"

UCLASS(Abstract, Blueprintable, meta = (DisplayName = "VN Audio Manager Base"))
class KISHIVISUALNOVEL_API UKishiRpyAudioManagerWidgetBase : public UKishiRpyManagerWidgetBase, public IRpyAudioManager
{
	GENERATED_BODY()
public:
	virtual void SetInterpreter_Implementation(const TScriptInterface<IRpyScriptInterpreter> &InInterpreter) override { UKishiRpyManagerWidgetBase::SetInterpreter_Implementation(InInterpreter); }
	virtual TScriptInterface<IRpyScriptInterpreter> GetInterpreter_Implementation() const override;
	virtual void OnInstructionComplete_Implementation() override;
	virtual void OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset) override;
	virtual void OnScriptComplete_Implementation() override;
	virtual void Reset_Implementation() override;
	virtual FRpyAudioManagerState GetAudioManagerState_Implementation() const override { return AudioState; }
	virtual void SetAudioManagerState_Implementation(const FRpyAudioManagerState &State) override;
	virtual ERpyInstructionResult PauseAudio_Implementation(float Timeout) override;
	virtual ERpyInstructionResult StopAudio_Implementation(FName Channel, FRpyAudioOptions Options) override;
	virtual ERpyInstructionResult QueueAudio_Implementation(FName Channel, FRpyAudio Audio, FRpyAudioOptions Options) override;
	virtual ERpyInstructionResult PlayAudio_Implementation(FName Channel, FRpyAudio Audio, FRpyAudioOptions Options) override;

protected:
	virtual void NativeOnInitialized() override;
	UPROPERTY(BlueprintReadOnly, Category = "Rpy|Audio")
	FRpyAudioManagerState AudioState;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Rpy|Audio")
	UAudioPlayer *AudioPlayer = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rpy|Audio")
	TMap<FName, USoundClass *> AudioChannels;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rpy|Audio")
	USoundWave *Blip = nullptr;
	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi Rpy Audio Manager Widget Base|Hooks")
	void OnPauseAudioRequested(float Timeout);
	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi Rpy Audio Manager Widget Base|Hooks")
	void OnStopAudioRequested(FName Channel, const FRpyAudioOptions &Options);
	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi Rpy Audio Manager Widget Base|Hooks")
	void OnQueueAudioRequested(FName Channel, const FRpyAudio &Audio, const FRpyAudioOptions &Options);
	UFUNCTION(BlueprintImplementableEvent, Category = "Kishi Rpy Audio Manager Widget Base|Hooks")
	void OnPlayAudioRequested(FName Channel, const FRpyAudio &Audio, const FRpyAudioOptions &Options);
	USoundWave *ResolveAudio(const FRpyAudio &Audio) const;
	void LogAudioFailure(const TCHAR *Operation, FName Channel, const FRpyAudio &Audio, const USoundWave *Sound) const;
};
