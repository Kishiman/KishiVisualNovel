#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Interfaces/RpyScriptInterpreterBaseManager.h"

#include "RpyChoiceManager.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API URpyChoiceManager : public URpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyChoiceManager : public IRpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
public:
  // get state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  FRpyChoiceManagerState GetChoiceManagerState() const;
  virtual FRpyChoiceManagerState GetChoiceManagerState_Implementation() const = 0;

  // set state
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void SetChoiceManagerState(const FRpyChoiceManagerState &State);
  virtual void SetChoiceManagerState_Implementation(const FRpyChoiceManagerState &State) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool Menu(const TArray<FString> &choices);
  virtual bool Menu_Implementation(const TArray<FString> &choices) = 0;

  // Handlers Interface

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void Choose(int idx);
  virtual void Choose_Implementation(int idx) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  FRpyChoiceManagerState GetCurrentChoiceState();
  virtual FRpyChoiceManagerState GetCurrentChoiceState_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void ClearAll();
  virtual void ClearAll_Implementation() = 0;
};

// UCLASS()
// class KISHIVISUALNOVEL_API URpyChoiceManagerLibrary : public UBlueprintFunctionLibrary
// {
//   GENERATED_BODY()
// public:
// protected:
// private:
// };
