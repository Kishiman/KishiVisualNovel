#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "CameraAnchorComponent.generated.h"

class UInteractingComponent;

UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent), BlueprintType)
class KISHIVISUALNOVEL_API UCameraAnchorComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  /** Turning speed (deg/sec) */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraAnchor")
  float TurnSpeed = 45.f;

  /** Mouse edge threshold in pixels of viewport*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraAnchor")
  float edgeMarginPxl = 50.0f;

  /** Mouse edge border in pixels of viewport*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraAnchor")
  float edgeBorderPxl = 10.0f;

  /** View limits */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraAnchor")
  float MinYaw = -45.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraAnchor")
  float MaxYaw = 45.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraAnchor")
  float MinPitch = -20.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraAnchor")
  float MaxPitch = 20.f;

  UCameraAnchorComponent();

  virtual void BeginPlay() override;

  virtual void Activate(bool bReset = false) override;
  virtual void Deactivate() override;

  /** Per-frame camera update while interacting */
  virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

protected:
  /** The camera we will rotate */
  UPROPERTY(BlueprintReadOnly, Category = "CameraAnchor")
  UCameraComponent *ControlledCamera = nullptr;

  /** Should we tick camera logic? */
  bool bIsAnchored = false;

  FRotator InitialRotation;
};
