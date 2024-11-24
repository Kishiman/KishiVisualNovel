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
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool Say(FName name, const FString &statement);
  virtual bool Say_Implementation(FName name, const FString &statement) = 0;
};

UCLASS()
class KISHIVISUALNOVEL_API URpyStatementManagerLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
protected:
private:
};
