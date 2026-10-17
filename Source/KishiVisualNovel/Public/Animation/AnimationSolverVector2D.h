#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "AnimationSolverVector2D.generated.h"

UENUM(BlueprintType)
enum class EInterpolationMethod : uint8
{
  None,
  Constant,
  Ease,
  Kinematic,
};

USTRUCT(BlueprintType)
struct KISHIVISUALNOVEL_API FAnimationSolverVector2D
{
  friend class UAnimationSolverVector2DLibrary;

  GENERATED_BODY()
public:
protected:
  UPROPERTY(BlueprintReadWrite)
  EInterpolationMethod method;

  float alpha;
  FVector2D start;
  FVector2D end;

  float speed;
  float ease;
};

UCLASS()
class KISHIVISUALNOVEL_API UAnimationSolverVector2DLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  // float
  UFUNCTION(BlueprintPure)
  static bool isPlaying(const FAnimationSolverVector2D &Target)
  {
    return Target.method != EInterpolationMethod::None;
  }

  UFUNCTION(BlueprintCallable)
  static bool Tick(UPARAM(ref) FAnimationSolverVector2D &Target, float deltaTime, FVector2D &out);

  UFUNCTION(BlueprintCallable)
  static void PlayConstant(UPARAM(ref) FAnimationSolverVector2D &Target, FVector2D start, FVector2D end, float time);

  UFUNCTION(BlueprintCallable)
  static void PlayEase(UPARAM(ref) FAnimationSolverVector2D &Target, FVector2D start, FVector2D end, float time, float ease = 2);
};