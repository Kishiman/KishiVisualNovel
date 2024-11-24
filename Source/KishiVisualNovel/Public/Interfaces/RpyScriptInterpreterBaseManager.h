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
};

UCLASS()
class KISHIVISUALNOVEL_API URpyScriptInterpreterBaseManagerLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintPure)
  static URpySession *GetSession(const TScriptInterface<IRpyScriptInterpreterBaseManager> &Target);

  UFUNCTION(BlueprintPure)
  static EInstructionRunTimeType GetRunTimeType(const TScriptInterface<IRpyScriptInterpreterBaseManager> &Target);

protected:
private:
};
