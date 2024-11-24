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
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool Menu(const TArray<FString> &choices);
  virtual bool Menu_Implementation(const TArray<FString> &choices) = 0;

  // Handlers Interface

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void Choose(int idx);
  virtual void Choose_Implementation(int idx) = 0;
};

// UCLASS()
// class KISHIVISUALNOVEL_API URpyChoiceManagerLibrary : public UBlueprintFunctionLibrary
// {
//   GENERATED_BODY()
// public:
// protected:
// private:
// };
