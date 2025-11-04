// IPlayerAnchor.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerAnchor.generated.h"

UINTERFACE(Blueprintable)
class KISHIVISUALNOVEL_API UPlayerAnchor : public UInterface
{
  GENERATED_BODY()
};

/**
 * Interface for any object the player can "anchor" to
 * (e.g., sit in chair, lean on wall, use computer terminal)
 */
class KISHIVISUALNOVEL_API IPlayerAnchor
{
  GENERATED_BODY()

public:
  /** Called when player begins using this anchor */
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerAnchor")
  void OnEnterAnchor(ACharacter *PlayerCharacter);

  /** Called when player stops using this anchor */
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerAnchor")
  void OnExitAnchor(ACharacter *PlayerCharacter);

  /** Returns the world transform where the player should snap to when anchoring */
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerAnchor")
  FTransform GetAnchorTransform() const;

  /** Returns the allowed yaw/pitch limits while anchored */
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerAnchor")
  void GetViewLimits(float &OutMinYaw, float &OutMaxYaw, float &OutMinPitch, float &OutMaxPitch) const;

  /** Whether the player can currently anchor to this (e.g., if seat is free) */
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerAnchor")
  bool CanAnchor(ACharacter *PlayerCharacter) const;
};
USTRUCT(BlueprintType)
struct FAnchorCameraResult
{
  GENERATED_BODY()

  UPROPERTY(BlueprintReadWrite, Category = "Camera")
  FVector2D CursorPos;
  UPROPERTY(BlueprintReadWrite, Category = "Camera")
  FTransform CameraTransform;
};

UCLASS()
class KISHIVISUALNOVEL_API UPlayerAnchorLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  /*
  DefaultImplementation
  */

  UFUNCTION(BlueprintPure, meta = (DefaultToSelf = "Anchor"), Category = "PlayerAnchor")
  FAnchorCameraResult UpdateAnchoredCameraAndCursor(
      const TScriptInterface<IPlayerAnchor> &Anchor,
      const UCameraComponent *Camera,
      const FVector2D &CursorPos,
      float ViewportWidth,
      float ViewportHeight,
      float Threshold,
      const FVector2D &TurnInput, // e.g., mouse delta or joystick axis
      float DeltaTime,
      float TurnSpeed = 45.f);

protected:
private:
};