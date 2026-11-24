#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "RpyScriptInterpreter.h"


class URpyInstruction;
class URpySession;

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

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreterBaseManager|Hooks", meta = (ToolTip = "Optional listener: respond when an instruction completes. The default does nothing."))
  void OnInstructionComplete();
  virtual void OnInstructionComplete_Implementation() {}

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreterBaseManager|Hooks", meta = (ToolTip = "Optional listener: respond to dialogue viseme updates. The default does nothing."))
  void OnDialogueViseme(FName name, EMouthViseme viseme, float timeOffset);
  virtual void OnDialogueViseme_Implementation(FName name, EMouthViseme viseme, float timeOffset) {}

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreterBaseManager|Hooks", meta = (ToolTip = "Optional listener: respond when the script completes. The default does nothing."))
  void OnScriptComplete();
  virtual void OnScriptComplete_Implementation() {}

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyScriptInterpreterBaseManager|Hooks", meta = (ToolTip = "Optional listener: the player asked to skip (a click outside dialogue). Finish the animations the script is waiting for at once, and report their completion. The default does nothing."))
  void OnSkipRequested();
  virtual void OnSkipRequested_Implementation() {}

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

  UFUNCTION(BlueprintPure, Category = "Rpy Script Interpreter Base Manager Library", meta = (DefaultToSelf = "Target", ToolTip = "The instruction that called the operation now running, or None outside one. An operation that returns Wait keeps it and passes it to ReportInstructionComplete, so a completion inside a <together> block is counted for the right command."))
  static URpyInstruction *GetExecutingInstruction(const TScriptInterface<IRpyScriptInterpreterBaseManager> &Target);

  UFUNCTION(BlueprintCallable, Category = "Rpy Script Interpreter Base Manager Library", meta = (DefaultToSelf = "Target", ToolTip = "Report that the work an operation returned Wait for has finished. Instruction is the one GetExecutingInstruction returned when the operation started; None means the current instruction. The session advances once the current instruction waits on nothing else, and ignores a completion that arrives after the script moved on. Returns false, after logging why, if the manager has no session, or if the completion was ignored."))
  static bool ReportInstructionComplete(const TScriptInterface<IRpyScriptInterpreterBaseManager> &Target, URpyInstruction *Instruction = nullptr);

  UFUNCTION(BlueprintCallable, Category = "Rpy Script Interpreter Base Manager Library", meta = (DefaultToSelf = "Target", ToolTip = "Ask the manager's interpreter to skip what the script is waiting for, as a click outside dialogue does (URpyScriptInterpreterLibrary::RequestSkip)."))
  static void RequestSkip(const TScriptInterface<IRpyScriptInterpreterBaseManager> &Target);

protected:
private:
};
