#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Player/RpyScriptInterpreterBaseManager.h"
#include "Script/RpyScriptTypes.h"

#include "RpyAudioManager.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API URpyAudioManager : public URpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyAudioManager : public IRpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
public:
  // get state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyAudioManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  FRpyAudioManagerState GetAudioManagerState() const;
  virtual FRpyAudioManagerState GetAudioManagerState_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyAudioManager::GetAudioManagerState")); return {}; }

  // set state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyAudioManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  void SetAudioManagerState(const FRpyAudioManagerState &State);
  virtual void SetAudioManagerState_Implementation(const FRpyAudioManagerState &State) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyAudioManager::SetAudioManagerState")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyAudioManager|Required", meta = (ToolTip = "Required: pause every channel, fading out over the timeout. Return Continue, or Failed, after logging why, if audio cannot be paused."))
  ERpyInstructionResult PauseAudio(float timeout);
  virtual ERpyInstructionResult PauseAudio_Implementation(float timeout) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyAudioManager::PauseAudio")); return ERpyInstructionResult::Failed; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyAudioManager|Required", meta = (ToolTip = "Required: stop the channel, fading out as the options say. Return Continue, or Failed, after logging why, if the channel cannot be stopped."))
  ERpyInstructionResult StopAudio(FName channel, FRpyAudioOptions options);
  virtual ERpyInstructionResult StopAudio_Implementation(FName channel, FRpyAudioOptions options) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyAudioManager::StopAudio")); return ERpyInstructionResult::Failed; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyAudioManager|Required", meta = (ToolTip = "Required: queue the sound on the channel after what is playing. Return Continue, or Failed, after logging why, if the channel or sound is missing."))
  ERpyInstructionResult QueueAudio(FName channel, FRpyAudio audio, FRpyAudioOptions options);
  virtual ERpyInstructionResult QueueAudio_Implementation(FName channel, FRpyAudio audio, FRpyAudioOptions options) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyAudioManager::QueueAudio")); return ERpyInstructionResult::Failed; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyAudioManager|Required", meta = (ToolTip = "Required: play the sound on the channel, replacing what is playing there. Return Continue, or Failed, after logging why, if the channel or sound is missing."))
  ERpyInstructionResult PlayAudio(FName channel, FRpyAudio audio, FRpyAudioOptions options);
  virtual ERpyInstructionResult PlayAudio_Implementation(FName channel, FRpyAudio audio, FRpyAudioOptions options) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyAudioManager::PlayAudio")); return ERpyInstructionResult::Failed; }
};

class UAudioPlayer;
class USoundWave;

/** Shared logic for IRpyAudioManager implementers: the widget Base calls these, and a component or Blueprint manager can call them the same way. The manager owns its UAudioPlayer and its FRpyAudioManagerState. */
UCLASS()
class KISHIVISUALNOVEL_API URpyAudioManagerLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintPure, Category = "Rpy Audio Manager Library", meta = (ToolTip = "Return the audio's sound: its loaded asset, else the asset at its path, else None."))
  static USoundWave *ResolveAudio(const FRpyAudio &Audio);

  UFUNCTION(BlueprintCallable, Category = "Rpy Audio Manager Library", meta = (DefaultToSelf = "Target", ToolTip = "Default PlayAudio: play the sound on the player's channel, replacing what is playing there. Returns Continue, or Failed after logging why. Record the channel in the manager's state when it succeeds."))
  static ERpyInstructionResult PlayAudio_Default(const TScriptInterface<IRpyAudioManager> &Target, UAudioPlayer *Player, FName Channel, const FRpyAudio &Audio, const FRpyAudioOptions &Options);

  UFUNCTION(BlueprintCallable, Category = "Rpy Audio Manager Library", meta = (DefaultToSelf = "Target", ToolTip = "Default QueueAudio: queue the sound on the player's channel after what is playing. Returns Continue, or Failed after logging why."))
  static ERpyInstructionResult QueueAudio_Default(const TScriptInterface<IRpyAudioManager> &Target, UAudioPlayer *Player, FName Channel, const FRpyAudio &Audio, const FRpyAudioOptions &Options);

  UFUNCTION(BlueprintCallable, Category = "Rpy Audio Manager Library", meta = (DefaultToSelf = "Target", ToolTip = "Default StopAudio: stop the player's channel, fading out as the options say. Returns Continue, or Failed after logging why. Remove the channel from the manager's state when it succeeds."))
  static ERpyInstructionResult StopAudio_Default(const TScriptInterface<IRpyAudioManager> &Target, UAudioPlayer *Player, FName Channel, const FRpyAudioOptions &Options);

  UFUNCTION(BlueprintCallable, Category = "Rpy Audio Manager Library", meta = (DefaultToSelf = "Target", ToolTip = "Default PauseAudio: pause every channel of the player, fading out over the timeout. Returns Continue, or Failed after logging why."))
  static ERpyInstructionResult PauseAudio_Default(const TScriptInterface<IRpyAudioManager> &Target, UAudioPlayer *Player, float Timeout);

  UFUNCTION(BlueprintCallable, Category = "Rpy Audio Manager Library", meta = (DefaultToSelf = "Target", ToolTip = "Default SetAudioManagerState: stop every channel in PreviousState, then call PlayAudio on the manager for every looping channel in State. Audio that plays once, such as a voice line, is not replayed. Clear the manager's own state before calling this."))
  static void SetAudioManagerState_Default(const TScriptInterface<IRpyAudioManager> &Target, UAudioPlayer *Player, const FRpyAudioManagerState &PreviousState, const FRpyAudioManagerState &State);

  UFUNCTION(BlueprintCallable, Category = "Rpy Audio Manager Library", meta = (ToolTip = "Play the dialogue blip as a one-shot on the 'sound' channel for a spoken (non-Neutral) viseme. A one-shot is not recorded in the saved state and does not cut off a sound the script started on that channel."))
  static void PlayDialogueBlip(UAudioPlayer *Player, USoundWave *Blip, EMouthViseme Viseme);

  /** The looping channels of State. A save restores only these: audio that plays once (a voice line, a sound) has finished or would restart mid-scene. */
  UFUNCTION(BlueprintPure, Category = "Rpy Audio Manager Library")
  static FRpyAudioManagerState OnlyLoopingAudio(const FRpyAudioManagerState &State);

  /** Record a channel that PlayAudio started, replacing the channel's previous entry. */
  static void RecordPlayedAudio(FRpyAudioManagerState &State, FName Channel, const FRpyAudio &Audio, const FRpyAudioOptions &Options);
  /** Remove a channel that StopAudio stopped. */
  static void RecordStoppedAudio(FRpyAudioManagerState &State, FName Channel);
};
