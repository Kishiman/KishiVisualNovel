#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Interfaces/RpyScriptInterpreterBaseManager.h"

#include "RpySceneManager.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API URpySceneManager : public URpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpySceneManager : public IRpyScriptInterpreterBaseManager
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool Scene(FRpyImage rpyImage, FRpySceneOptions options);
  virtual bool Scene_Implementation(FRpyImage rpyImage, FRpySceneOptions options) = 0;
};

// UCLASS()
// class KISHIVISUALNOVEL_API URpySceneManagerLibrary : public UBlueprintFunctionLibrary
// {
//   GENERATED_BODY()
// public:
// protected:
// private:

// };
