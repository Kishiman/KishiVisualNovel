#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Rpy/RpyScript.h"

#include "RpyCameraAnchor.generated.h"

class IRpyAudioManager;
class IRpyChoiceManager;
class IRpySceneManager;
class IRpyShowManager;
class IRpyStatementManager;

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API URpyCameraAnchor : public UInterface
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyCameraAnchor
{
  GENERATED_BODY()
public:
  // instructions

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  AActor *GetActor() const;
  virtual AActor *GetActor_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  UCameraComponent *GetCameraComponent() const;
  virtual UCameraComponent *GetCameraComponent_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  FName GetAnchorLayer() const;
  virtual FName GetAnchorLayer_Implementation() const = 0;
};

UCLASS()
class KISHIVISUALNOVEL_API URpyCameraAnchorLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  /*
  DefaultImplementation
  */

  UFUNCTION(BlueprintPure, Category = "Default|RpyCameraAnchor")
  static TScriptInterface<IRpyCameraAnchor> FindAnchorByLayerName(UWorld *World, FName Layer);

protected:
private:
};
