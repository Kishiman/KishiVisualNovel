#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Player/RpyScriptInterpreterBaseManager.h"
#include "Script/RpyScriptTypes.h"
#include "Showables/Showable.h"
#include "Settings/KishiVisualNovelSettings.h"

#include "RpyShowManager.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API URpyShowManager : public URpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyShowManager : public IRpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
public:
  // get state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyShowManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  FRpyShowManagerState GetShowManagerState() const;
  virtual FRpyShowManagerState GetShowManagerState_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyShowManager::GetShowManagerState")); return {}; }

  // set state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyShowManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  void SetShowManagerState(const FRpyShowManagerState &State);
  virtual void SetShowManagerState_Implementation(const FRpyShowManagerState &State) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyShowManager::SetShowManagerState")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyShowManager|Required", meta = (ToolTip = "Required: show the image under its tag, or update it if that tag is already shown. Return Continue once it is shown (a transition may still be playing), Wait if you will report completion through the interpreter yourself, or Failed, after logging why, if it cannot be shown."))
  ERpyInstructionResult Show(FRpyImage rpyImage, FRpySceneOptions options);
  virtual ERpyInstructionResult Show_Implementation(FRpyImage rpyImage, FRpySceneOptions options) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyShowManager::Show")); return ERpyInstructionResult::Failed; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyShowManager|Required", meta = (ToolTip = "Required: hide the image with this tag. Return Continue when it is hidden or when no image has that tag, or Failed, after logging why, if it cannot be hidden."))
  ERpyInstructionResult Hide(FName tag, FRpySceneOptions options);
  virtual ERpyInstructionResult Hide_Implementation(FName tag, FRpySceneOptions options) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyShowManager::Hide")); return ERpyInstructionResult::Failed; }
};

/** Shared logic for IRpyShowManager implementers: the widget Base calls these, and a component or Blueprint manager can call them the same way. */
UCLASS()
class KISHIVISUALNOVEL_API URpyShowManagerLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable, Category = "Rpy Show Manager Library", meta = (DefaultToSelf = "Target", ToolTip = "Default SetShowManagerState: call Show on the manager for every saved image, in order. Remove the shown images and clear the manager's own state before calling this."))
  static void SetShowManagerState_Default(const TScriptInterface<IRpyShowManager> &Target, const FRpyShowManagerState &State);

  UFUNCTION(BlueprintPure, Category = "Rpy Show Manager Library", meta = (ToolTip = "Convert script scene options into the options IShowable::Show and IShowable::Hide take."))
  static FKishiShowableOptions MakeShowableOptions(const FRpySceneOptions &Options);

  UFUNCTION(BlueprintPure, Category = "Rpy Show Manager Library", meta = (ToolTip = "Return Options without its transition (no type, sequence, direction or duration), so the image appears at once. The state restore functions use it, so a loaded save cuts to its images instead of replaying their transitions."))
  static FRpySceneOptions WithoutTransition(const FRpySceneOptions &Options);

  UFUNCTION(BlueprintCallable, Category = "Rpy Show Manager Library", meta = (ToolTip = "Return the showable class that presents Asset in Space: the first match in Overrides, else in the registry in Project Settings, walking up Asset's class hierarchy. Returns None when Asset is None or nothing matches."))
  static TSubclassOf<UObject> ResolveShowableClass(const UObject *Asset, EKishiShowableSpace Space, const TArray<FKishiShowableClassEntry> &Overrides);

  UFUNCTION(BlueprintPure, Category = "Rpy Show Manager Library", meta = (ToolTip = "True when Showable can switch to the image in place: it is of ShowableClass, and the image's asset is the layered sprite it already shows. Sprites and videos are recreated so the new image plays its own show transition."))
  static bool CanReuseShowable(UObject *Showable, TSubclassOf<UObject> ShowableClass, const FRpyImage &Image);

  UFUNCTION(BlueprintCallable, Category = "Rpy Show Manager Library", meta = (ToolTip = "Forward a dialogue viseme to the speaker's showable if it implements ILayeredSpriteHandler. Does nothing otherwise."))
  static void ForwardSpeakerViseme(UObject *Speaker, EMouthViseme Viseme, float TimeOffset);
};
