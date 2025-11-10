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
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void StartUp();
  virtual void StartUp_Implementation() = 0;
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void Shutdown();
  virtual void Shutdown_Implementation() = 0;
  // menus
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool getSaveMenuVisible() const;
  virtual bool getSaveMenuVisible_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void setSaveMenuVisible(bool bVisible) const;
  virtual void setSaveMenuVisible_Implementation(bool bVisible) const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool getLoadMenuVisible() const;
  virtual bool getLoadMenuVisible_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void setLoadMenuVisible(bool bVisible) const;
  virtual void setLoadMenuVisible_Implementation(bool bVisible) const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool getConfigMenuVisible() const;
  virtual bool getConfigMenuVisible_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void setConfigMenuVisible(bool bVisible) const;
  virtual void setConfigMenuVisible_Implementation(bool bVisible) const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool getBackLogMenuVisible() const;
  virtual bool getBackLogMenuVisible_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void setBackLogMenuVisible(bool bVisible) const;
  virtual void setBackLogMenuVisible_Implementation(bool bVisible) const = 0;

  // instructions

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  URpySession *GetSession() const;
  virtual URpySession *GetSession_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  TScriptInterface<IRpyAudioManager> GetAudioManager() const;
  virtual TScriptInterface<IRpyAudioManager> GetAudioManager_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  TScriptInterface<IRpyChoiceManager> GetChoiceManager() const;
  virtual TScriptInterface<IRpyChoiceManager> GetChoiceManager_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  TScriptInterface<IRpySceneManager> GetSceneManager() const;
  virtual TScriptInterface<IRpySceneManager> GetSceneManager_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  TScriptInterface<IRpyShowManager> GetShowManager() const;
  virtual TScriptInterface<IRpyShowManager> GetShowManager_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  TScriptInterface<IRpyStatementManager> GetStatementManager() const;
  virtual TScriptInterface<IRpyStatementManager> GetStatementManager_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void OnInstructionComplete(EInstructionRunTimeType type);
  virtual void OnInstructionComplete_Implementation(EInstructionRunTimeType type) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void OnDialogueViseme(FName name, EMouthViseme viseme, float timeOffset);
  virtual void OnDialogueViseme_Implementation(FName name, EMouthViseme viseme, float timeOffset) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
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
  static void IOnInstructionComplete_Default(const TScriptInterface<IRpyScriptInterpreter> &Target, EInstructionRunTimeType type);

  UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Target"), Category = "Default")
  static void IOnDialogueViseme_Default(const TScriptInterface<IRpyScriptInterpreter> &Target, FName name, EMouthViseme viseme, float timeOffset);
};
