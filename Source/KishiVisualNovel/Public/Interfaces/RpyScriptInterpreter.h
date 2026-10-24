#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Utils/StringUtils.h"

#include "Rpy/RpyScript.h"

#include "RpyScriptInterpreter.generated.h"

class IRpyAudioManager;
class IRpyChoiceManager;
class IRpySceneManager;
class IRpyShowManager;
class IRpyStatementManager;

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API URpyScriptInterpreter : public UInterface
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyScriptInterpreter
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: implement this function to start the interpreter."))
  void StartUp();
  virtual void StartUp_Implementation() { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::StartUp")); }
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: implement this function to shut down the interpreter."))
  void Shutdown();
  virtual void Shutdown_Implementation() { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::Shutdown")); }
  // menus
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: return whether the save menu is visible."))
  bool getSaveMenuVisible() const;
  virtual bool getSaveMenuVisible_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::getSaveMenuVisible")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: set the save menu visibility."))
  void setSaveMenuVisible(bool bVisible) const;
  virtual void setSaveMenuVisible_Implementation(bool bVisible) const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::setSaveMenuVisible")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: return whether the load menu is visible."))
  bool getLoadMenuVisible() const;
  virtual bool getLoadMenuVisible_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::getLoadMenuVisible")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: set the load menu visibility."))
  void setLoadMenuVisible(bool bVisible) const;
  virtual void setLoadMenuVisible_Implementation(bool bVisible) const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::setLoadMenuVisible")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: return whether the config menu is visible."))
  bool getConfigMenuVisible() const;
  virtual bool getConfigMenuVisible_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::getConfigMenuVisible")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: set the config menu visibility."))
  void setConfigMenuVisible(bool bVisible) const;
  virtual void setConfigMenuVisible_Implementation(bool bVisible) const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::setConfigMenuVisible")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: return whether the backlog menu is visible."))
  bool getBackLogMenuVisible() const;
  virtual bool getBackLogMenuVisible_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::getBackLogMenuVisible")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: set the backlog menu visibility."))
  void setBackLogMenuVisible(bool bVisible) const;
  virtual void setBackLogMenuVisible_Implementation(bool bVisible) const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::setBackLogMenuVisible")); }

  // instructions

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: store the session used by this interpreter."))
  void SetSession(URpySession *InSession);
  virtual void SetSession_Implementation(URpySession *InSession) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::SetSession")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: return this interpreter's current session."))
  URpySession *GetSession() const;
  virtual URpySession *GetSession_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::GetSession")); return nullptr; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: return the audio manager used by this interpreter."))
  TScriptInterface<IRpyAudioManager> GetAudioManager() const;
  virtual TScriptInterface<IRpyAudioManager> GetAudioManager_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::GetAudioManager")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: return the choice manager used by this interpreter."))
  TScriptInterface<IRpyChoiceManager> GetChoiceManager() const;
  virtual TScriptInterface<IRpyChoiceManager> GetChoiceManager_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::GetChoiceManager")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: return the scene manager used by this interpreter."))
  TScriptInterface<IRpySceneManager> GetSceneManager() const;
  virtual TScriptInterface<IRpySceneManager> GetSceneManager_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::GetSceneManager")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: return the show manager used by this interpreter."))
  TScriptInterface<IRpyShowManager> GetShowManager() const;
  virtual TScriptInterface<IRpyShowManager> GetShowManager_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::GetShowManager")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: return the statement manager used by this interpreter."))
  TScriptInterface<IRpyStatementManager> GetStatementManager() const;
  virtual TScriptInterface<IRpyStatementManager> GetStatementManager_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::GetStatementManager")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required handler: advance the interpreter after asynchronous instruction work completes."))
  void HandleInstructionComplete();
  virtual void HandleInstructionComplete_Implementation() { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::HandleInstructionComplete")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Hooks", meta = (ToolTip = "Optional listener: respond to dialogue viseme updates."))
  void OnDialogueViseme(FName name, EMouthViseme viseme, float timeOffset);
  virtual void OnDialogueViseme_Implementation(FName name, EMouthViseme viseme, float timeOffset) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Hooks", meta = (ToolTip = "Optional listener: respond when the script completes."))
  void OnScriptComplete();
  virtual void OnScriptComplete_Implementation() = 0;
};

UCLASS(MinimalAPI)
class URpyScriptInterpreterImplementation : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  /*
  Default Implementation
  */

  UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Target"), Category = "Default")
  static void IHandleInstructionComplete_Default(const TScriptInterface<IRpyScriptInterpreter> &Target);

  UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Target"), Category = "Default")
  static void IOnDialogueViseme_Default(const TScriptInterface<IRpyScriptInterpreter> &Target, FName name, EMouthViseme viseme, float timeOffset);
};
