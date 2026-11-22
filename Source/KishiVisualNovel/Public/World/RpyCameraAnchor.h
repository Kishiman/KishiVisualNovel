#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Script/RpyScript.h"

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
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyCameraAnchor|Required", meta = (ToolTip = "Required: return the actor this anchor belongs to."))
  AActor *GetActor() const;
  virtual AActor *GetActor_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyCameraAnchor::GetActor")); return nullptr; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyCameraAnchor|Required", meta = (ToolTip = "Required: return this anchor's camera component, or None."))
  UCameraComponent *GetCameraComponent() const;
  virtual UCameraComponent *GetCameraComponent_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyCameraAnchor::GetCameraComponent")); return nullptr; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpyCameraAnchor|Required", meta = (ToolTip = "Required: return the layer name that a scene instruction's layer attribute uses to find this anchor."))
  FName GetAnchorLayer() const;
  virtual FName GetAnchorLayer_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpyCameraAnchor::GetAnchorLayer")); return NAME_None; }
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
