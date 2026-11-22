#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Utils/StringUtils.h"

#include "Script/RpyScript.h"

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
  bool GetSaveMenuVisible() const;
  virtual bool GetSaveMenuVisible_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::GetSaveMenuVisible")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: set the save menu visibility."))
  void SetSaveMenuVisible(bool bVisible);
  virtual void SetSaveMenuVisible_Implementation(bool bVisible) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::SetSaveMenuVisible")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: return whether the load menu is visible."))
  bool GetLoadMenuVisible() const;
  virtual bool GetLoadMenuVisible_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::GetLoadMenuVisible")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: set the load menu visibility."))
  void SetLoadMenuVisible(bool bVisible);
  virtual void SetLoadMenuVisible_Implementation(bool bVisible) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::SetLoadMenuVisible")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: return whether the config menu is visible."))
  bool GetConfigMenuVisible() const;
  virtual bool GetConfigMenuVisible_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::GetConfigMenuVisible")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: set the config menu visibility."))
  void SetConfigMenuVisible(bool bVisible);
  virtual void SetConfigMenuVisible_Implementation(bool bVisible) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::SetConfigMenuVisible")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: return whether the backlog menu is visible."))
  bool GetBackLogMenuVisible() const;
  virtual bool GetBackLogMenuVisible_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::GetBackLogMenuVisible")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Required", meta = (ToolTip = "Required: set the backlog menu visibility."))
  void SetBackLogMenuVisible(bool bVisible);
  virtual void SetBackLogMenuVisible_Implementation(bool bVisible) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreter::SetBackLogMenuVisible")); }

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

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Hooks", meta = (ToolTip = "Optional listener: respond to dialogue viseme updates. The default does nothing."))
  void OnDialogueViseme(FName name, EMouthViseme viseme, float timeOffset);
  virtual void OnDialogueViseme_Implementation(FName name, EMouthViseme viseme, float timeOffset) {}

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreter|Hooks", meta = (ToolTip = "Optional listener: respond when the script completes. The default does nothing."))
  void OnScriptComplete();
  virtual void OnScriptComplete_Implementation() {}
};

/** Shared logic for IRpyScriptInterpreter implementers. The VN player Base calls these from its _Implementation overrides; a Blueprint or C++ interpreter can call them the same way. */
UCLASS()
class KISHIVISUALNOVEL_API URpyScriptInterpreterLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable, Category = "Rpy Script Interpreter Library", meta = (DefaultToSelf = "Target", ToolTip = "Default HandleInstructionComplete: notify every manager's OnInstructionComplete, then advance the session if the current instruction returned Wait, and shut the interpreter down when the script ends."))
  static void HandleInstructionComplete_Default(const TScriptInterface<IRpyScriptInterpreter> &Target);

  UFUNCTION(BlueprintCallable, Category = "Rpy Script Interpreter Library", meta = (DefaultToSelf = "Target", ToolTip = "Default OnDialogueViseme: forward the viseme to the audio and show managers."))
  static void OnDialogueViseme_Default(const TScriptInterface<IRpyScriptInterpreter> &Target, FName name, EMouthViseme viseme, float timeOffset);
};
