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
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IPlayerAnchor|Hooks", meta = (ToolTip = "Optional: called when PlayerCharacter starts using this anchor. The default does nothing."))
  void OnEnterAnchor(ACharacter *PlayerCharacter);
  virtual void OnEnterAnchor_Implementation(ACharacter *PlayerCharacter) {}

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IPlayerAnchor|Hooks", meta = (ToolTip = "Optional: called when PlayerCharacter stops using this anchor. The default does nothing."))
  void OnExitAnchor(ACharacter *PlayerCharacter);
  virtual void OnExitAnchor_Implementation(ACharacter *PlayerCharacter) {}

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IPlayerAnchor|Required", meta = (ToolTip = "Required: return the world transform the player snaps to when anchoring."))
  FTransform GetAnchorTransform() const;
  virtual FTransform GetAnchorTransform_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IPlayerAnchor::GetAnchorTransform")); return FTransform::Identity; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IPlayerAnchor|Required", meta = (ToolTip = "Required: return the yaw and pitch limits, in degrees, while the player is anchored."))
  void GetViewLimits(float &OutMinYaw, float &OutMaxYaw, float &OutMinPitch, float &OutMaxPitch) const;
  virtual void GetViewLimits_Implementation(float &OutMinYaw, float &OutMaxYaw, float &OutMinPitch, float &OutMaxPitch) const
  {
    UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IPlayerAnchor::GetViewLimits"));
    // No limits, so a missing implementation never locks the view.
    OutMinYaw = -180.f;
    OutMaxYaw = 180.f;
    OutMinPitch = -90.f;
    OutMaxPitch = 90.f;
  }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IPlayerAnchor|Required", meta = (ToolTip = "Required: return whether PlayerCharacter can anchor here now, for example whether the seat is free."))
  bool CanAnchor(ACharacter *PlayerCharacter) const;
  virtual bool CanAnchor_Implementation(ACharacter *PlayerCharacter) const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IPlayerAnchor::CanAnchor")); return false; }
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

  /** Returns Actor itself when it implements IPlayerAnchor, else its first component that does, else None. */
  UFUNCTION(BlueprintPure, Category = "PlayerAnchor")
  static TScriptInterface<IPlayerAnchor> FindPlayerAnchor(AActor *Actor);

  UFUNCTION(BlueprintPure, meta = (DefaultToSelf = "Anchor"), Category = "PlayerAnchor")
  static FAnchorCameraResult UpdateAnchoredCameraAndCursor(
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