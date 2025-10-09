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
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  FRpyStatementManagerState GetStatementManagerState() const;
  virtual FRpyStatementManagerState GetStatementManagerState_Implementation() const = 0;

  // set state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void SetStatementManagerState(const FRpyStatementManagerState &State);
  virtual void SetStatementManagerState_Implementation(const FRpyStatementManagerState &State) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool Say(FName name, const FString &statement);
  virtual bool Say_Implementation(FName name, const FString &statement) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  FRpyStatementManagerState GetCurrentStatementState();
  virtual FRpyStatementManagerState GetCurrentStatementState_Implementation() const = 0;
};

// UCLASS()
// class KISHIVISUALNOVEL_API URpyStatementManagerLibrary : public UBlueprintFunctionLibrary
// {
//   GENERATED_BODY()
// public:
// protected:
// private:
// };
