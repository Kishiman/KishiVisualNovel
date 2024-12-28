#include "Utils/VectorUtils.h"
#include "Kismet/KismetMathLibrary.h"

FVector2D UVectorUtils::GetSize(UPaperSprite *Target)
{
    return Target->GetSourceSize();
}
FVector2D UVectorUtils::ClampPreserveRatio(FVector2D Target, FVector2D Max)
{
    if (Max.X == 0 && Max.Y == 0)
    {
        // Max size is invalid, return original vector
        return Target;
    }
    if (Target.X == 0 || Target.Y == 0)
    {
        // No ratio, standard Clamp
        return FVector2D(Max.X == 0 ? Target.X : FMath::Clamp(Target.X, 0.f, Max.X), Max.Y == 0 ? Target.Y : FMath::Clamp(Target.Y, 0.f, Max.Y));
    }
    const float AspectRatio = Target.X / Target.Y;
    float scale = 1;
    if (Max.X && Max.X < Target.X)
    {
        scale = Max.X / Target.X;
    }
    if (Max.Y && Max.Y < Target.Y)
    {
        scale = FMath::Min(scale, Max.Y / Target.Y);
    }
    Target.X *= scale;
    Target.Y *= scale;

    return Target;
}

FVector2D UVectorUtils::ClampPreserveRatioMin(FVector2D Target, FVector2D Min)
{
    if (Min.X == 0 && Min.Y == 0)
    {
        // Min size is 0, return original vector
        return Target;
    }
    if (Target.X == 0 || Target.Y == 0)
    {
        // No ratio, standard Reverse Clamp
        return FVector2D(Target.X == 0 ? Target.X : FMath::Max(Target.X, Min.X), Target.Y == 0 ? Target.Y : FMath::Max(Target.Y, Min.Y));
    }

    const float AspectRatio = Target.X / Target.Y;
    float scale = 1;
    if (Min.X && Min.X > Target.X)
    {
        scale = Min.X / Target.X;
    }
    if (Min.Y && Min.Y > Target.Y)
    {
        scale = FMath::Max(scale, Min.Y / Target.Y);
    }
    Target.X *= scale;
    Target.Y *= scale;

    return Target;
}

FVector2D UVectorUtils::ClampPreserveRatioMinMax(FVector2D Target, FVector2D Min, FVector2D Max)
{
    auto MinClamp = ClampPreserveRatioMin(Target, Min);
    auto MaxClamp = ClampPreserveRatio(MinClamp, Max);
    return MaxClamp;
}