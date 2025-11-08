#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "RpyScriptInterpreter.h"

#include "RpyScriptInterpreterBaseManager.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API URpyScriptInterpreterBaseManager : public UInterface
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  TScriptInterface<IRpyScriptInterpreter> GetInterpreter() const;
  virtual TScriptInterface<IRpyScriptInterpreter> GetInterpreter_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void OnInstructionComplete(EInstructionRunTimeType type);
  virtual void OnInstructionComplete_Implementation(EInstructionRunTimeType type) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void OnDialogueViseme(FName name, EMouthViseme viseme, float timeOffset);
  virtual void OnDialogueViseme_Implementation(FName name, EMouthViseme viseme, float timeOffset) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void OnScriptComplete();
  virtual void OnScriptComplete_Implementation() = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void Reset();
  virtual void Reset_Implementation() = 0;
};

UCLASS()
class KISHIVISUALNOVEL_API URpyScriptInterpreterBaseManagerLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintPure, meta = (DefaultToSelf = "Target"))
  static URpySession *GetSession(const TScriptInterface<IRpyScriptInterpreterBaseManager> &Target);

  UFUNCTION(BlueprintPure, meta = (DefaultToSelf = "Target"))
  static EInstructionRunTimeType GetRunTimeType(const TScriptInterface<IRpyScriptInterpreterBaseManager> &Target);

protected:
private:
};
