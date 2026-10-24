#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Interfaces/RpyScriptInterpreterBaseManager.h"

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

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyStatementManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  bool Say(FName name, const FString &statement);
  virtual bool Say_Implementation(FName name, const FString &statement) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyStatementManager::Say")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyStatementManager|Required", meta = (ToolTip = "Required: implement this manager operation."))
  FRpyStatementManagerState GetCurrentStatementState();
  virtual FRpyStatementManagerState GetCurrentStatementState_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyStatementManager::GetCurrentStatementState")); return {}; }
};

// UCLASS()
// class KISHIVISUALNOVEL_API URpyStatementManagerLibrary : public UBlueprintFunctionLibrary
// {
//   GENERATED_BODY()
// public:
// protected:
// private:
// };
