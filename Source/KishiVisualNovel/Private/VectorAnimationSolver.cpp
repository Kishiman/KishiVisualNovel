#pragma once

#include "AnimationSolverVector2D.h"

bool UAnimationSolverVector2DLibrary::Tick(FAnimationSolverVector2D &Target, float deltaTime, FVector2D &out)
{
  if (Target.method == EInterpolationMethod::None)
    return false;
  if (Target.alpha >= 1)
  {
    out = Target.end;
    Target.method = EInterpolationMethod::None;
    return false;
  }
  Target.alpha += Target.speed * deltaTime;

  // Clamp Alpha between 0 and 1
  Target.alpha = FMath::Clamp(Target.alpha, 0.0f, 1.0f);
  if (Target.alpha >= 1)
  {
    out = Target.end;
    Target.method = EInterpolationMethod::None;
    return true;
  }

  switch (Target.method)
  {
  case EInterpolationMethod::Constant:
    // Linear interpolation
    out = FMath::Lerp(Target.start, Target.end, Target.alpha);
    return false;

  case EInterpolationMethod::Ease:

    float adjustedAlpha = FMath::InterpEaseInOut(0.0f, 1.0f, Target.alpha, Target.ease);
    out = FMath::Lerp(Target.start, Target.end, adjustedAlpha);
    return false;
  }
  return false;
}
void UAnimationSolverVector2DLibrary::PlayConstant(FAnimationSolverVector2D &Target, FVector2D start, FVector2D end, float time)
{
  Target.method = EInterpolationMethod::Constant;
  Target.start = start;
  Target.end = end;
  Target.alpha = 0;
  if (time <= 0)
  {
    Target.alpha = 1;
    Target.speed = 0;
  }
  else
  {
    Target.speed = 1 / time;
  }
  return;
}
void UAnimationSolverVector2DLibrary::PlayEase(FAnimationSolverVector2D &Target, FVector2D start, FVector2D end, float time, float ease)
{
  Target.method = EInterpolationMethod::Ease;
  Target.start = start;
  Target.end = end;
  Target.ease = ease;
  Target.alpha = 0;
  if (time <= 0)
  {
    Target.speed = 120;
  }
  else
  {
    Target.speed = 1 / time;
  }
  return;
}
