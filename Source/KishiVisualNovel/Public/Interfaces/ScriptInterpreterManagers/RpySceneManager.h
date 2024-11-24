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

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool Show(FRpyImage rpyImage, FRpySceneOptions options);
  virtual bool Show_Implementation(FRpyImage rpyImage, FRpySceneOptions options) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool Hide(FName tag, FRpySceneOptions options);
  virtual bool Hide_Implementation(FName tag, FRpySceneOptions options) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool ShowLayeredImage(FRpyLayeredImage rpyLayeredImage, const FString &attribute, FRpySceneOptions options);
  virtual bool ShowLayeredImage_Implementation(FRpyLayeredImage rpyLayeredImage, const FString &attribute, FRpySceneOptions options) = 0;
};

// UCLASS()
// class KISHIVISUALNOVEL_API URpySceneManagerLibrary : public UBlueprintFunctionLibrary
// {
//   GENERATED_BODY()
// public:
// protected:
// private:
// };
