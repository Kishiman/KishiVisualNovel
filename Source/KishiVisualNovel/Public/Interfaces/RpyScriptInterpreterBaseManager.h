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
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreterBaseManager|Required", meta = (ToolTip = "Required: store the interpreter that owns this manager."))
  void SetInterpreter(const TScriptInterface<IRpyScriptInterpreter> &InInterpreter);
  virtual void SetInterpreter_Implementation(const TScriptInterface<IRpyScriptInterpreter> &InInterpreter) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreterBaseManager::SetInterpreter")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreterBaseManager|Required", meta = (ToolTip = "Required: return the interpreter that owns this manager."))
  TScriptInterface<IRpyScriptInterpreter> GetInterpreter() const;
  virtual TScriptInterface<IRpyScriptInterpreter> GetInterpreter_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreterBaseManager::GetInterpreter")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreterBaseManager|Hooks", meta = (ToolTip = "Optional listener: respond when an instruction completes."))
  void OnInstructionComplete();
  virtual void OnInstructionComplete_Implementation() = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreterBaseManager|Hooks", meta = (ToolTip = "Optional listener: respond to dialogue viseme updates."))
  void OnDialogueViseme(FName name, EMouthViseme viseme, float timeOffset);
  virtual void OnDialogueViseme_Implementation(FName name, EMouthViseme viseme, float timeOffset) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreterBaseManager|Hooks", meta = (ToolTip = "Optional listener: respond when the script completes."))
  void OnScriptComplete();
  virtual void OnScriptComplete_Implementation() = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreterBaseManager|Required", meta = (ToolTip = "Required: reset this manager to its initial state."))
  void Reset();
  virtual void Reset_Implementation() { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyScriptInterpreterBaseManager::Reset")); }
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
