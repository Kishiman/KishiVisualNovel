#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Player/RpyScriptInterpreterBaseManager.h"
#include "Script/RpyScriptTypes.h"

#include "RpyStatementManager.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API URpyStatementManager : public URpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyStatementManager : public IRpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
public:
  // get state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyStatementManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  FRpyStatementManagerState GetStatementManagerState() const;
  virtual FRpyStatementManagerState GetStatementManagerState_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyStatementManager::GetStatementManagerState")); return {}; }

  // set state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyStatementManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  void SetStatementManagerState(const FRpyStatementManagerState &State);
  virtual void SetStatementManagerState_Implementation(const FRpyStatementManagerState &State) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyStatementManager::SetStatementManagerState")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyStatementManager|Required", meta = (ToolTip = "Required: display the line. Return Wait and report completion through the interpreter when the player has read it, Continue only for a line that should not wait, or Failed, after logging why, if it cannot be displayed."))
  ERpyInstructionResult Say(FName name, const FString &statement);
  virtual ERpyInstructionResult Say_Implementation(FName name, const FString &statement) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyStatementManager::Say")); return ERpyInstructionResult::Failed; }
};

// UCLASS()
// class KISHIVISUALNOVEL_API URpyStatementManagerLibrary : public UBlueprintFunctionLibrary
// {
//   GENERATED_BODY()
// public:
// protected:
// private:
// };
