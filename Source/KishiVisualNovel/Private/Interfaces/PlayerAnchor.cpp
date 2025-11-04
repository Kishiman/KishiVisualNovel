#include "Interfaces/PlayerAnchor.h"

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"

FAnchorCameraResult UPlayerAnchorLibrary::UpdateAnchoredCameraAndCursor(const TScriptInterface<IPlayerAnchor> &Anchor, const UCameraComponent *Camera, const FVector2D &CursorPos, float ViewportWidth, float ViewportHeight, float Threshold, const FVector2D &TurnInput, float DeltaTime, float TurnSpeed)
{
  FAnchorCameraResult Result;
  Result.CursorPos = CursorPos;
  Result.CameraTransform = Camera ? Camera->GetComponentTransform() : FTransform::Identity;

  if (!Anchor || !Camera)
    return Result;

  // Get yaw/pitch limits from anchor
  float MinYaw, MaxYaw, MinPitch, MaxPitch;
  IPlayerAnchor::Execute_GetViewLimits(Anchor.GetObject(), MinYaw, MaxYaw, MinPitch, MaxPitch);

  // Current rotation
  FRotator CamRot = Camera->GetComponentRotation();

  // --- Step 1: Move cursor based on turn input ---
  FVector2D Cursor = CursorPos + TurnInput * 10.f; // adjust sensitivity
  Cursor.X = FMath::Clamp(Cursor.X, 0.f, ViewportWidth);
  Cursor.Y = FMath::Clamp(Cursor.Y, 0.f, ViewportHeight);
  Result.CursorPos = Cursor;

  // --- Step 2: Edge detection to rotate camera ---
  float EdgeYawInput = 0.f;
  float EdgePitchInput = 0.f;

  if (Cursor.X < Threshold)
    EdgeYawInput = -1.f;
  else if (Cursor.X > ViewportWidth - Threshold)
    EdgeYawInput = 1.f;

  if (Cursor.Y < Threshold)
    EdgePitchInput = 1.f; // look up
  else if (Cursor.Y > ViewportHeight - Threshold)
    EdgePitchInput = -1.f; // look down

  // --- Step 3: Apply rotation ---
  float DeltaYaw = EdgeYawInput * TurnSpeed * DeltaTime;
  float DeltaPitch = EdgePitchInput * TurnSpeed * DeltaTime;

  CamRot.Yaw += DeltaYaw;
  CamRot.Pitch += DeltaPitch;

  // --- Step 4: Clamp to anchor limits ---
  CamRot.Yaw = FMath::Clamp(CamRot.Yaw, MinYaw, MaxYaw);
  CamRot.Pitch = FMath::Clamp(CamRot.Pitch, MinPitch, MaxPitch);

  Result.CameraTransform.SetLocation(Camera->GetComponentLocation());
  Result.CameraTransform.SetRotation(CamRot.Quaternion());

  return Result;
}