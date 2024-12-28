#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
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
};

// UCLASS()
// class KISHIVISUALNOVEL_API URpyScriptInterpreterLibrary : public UBlueprintFunctionLibrary
// {
//   GENERATED_BODY()
// public:
// protected:
// private:
// };
