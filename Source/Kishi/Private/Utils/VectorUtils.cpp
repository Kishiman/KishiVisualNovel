#include "Utils/VectorUtils.h"
#include "Kismet/KismetMathLibrary.h"

FVector2D UVectorUtils::GetSize(UPaperSprite *Target)
{
    return Target->GetSourceSize();
}

FVector2D UVectorUtils::ScaleResolutionScreen(FVector2D Source, FVector2D Target, EScalingMethod Method)
{
    switch (Method)
    {
    case EScalingMethod::Crop:
    {
        float SourceAspect = Source.X / Source.Y;
        float TargetAspect = Target.X / Target.Y;

        if (SourceAspect > TargetAspect)
        {
            // Crop width to match the target aspect ratio
            float NewWidth = Target.Y * SourceAspect;
            return FVector2D(NewWidth, Target.Y);
        }
        else
        {
            // Crop height to match the target aspect ratio
            float NewHeight = Target.X / SourceAspect;
            return FVector2D(Target.X, NewHeight);
        }
    }
    case EScalingMethod::Fit:
    {
        float SourceAspect = Source.X / Source.Y;
        float TargetAspect = Target.X / Target.Y;

        if (SourceAspect > TargetAspect)
        {
            // Fit width, adjust height to maintain aspect ratio
            float NewHeight = Target.X / SourceAspect;
            return FVector2D(Target.X, NewHeight);
        }
        else
        {
            // Fit height, adjust width to maintain aspect ratio
            float NewWidth = Target.Y * SourceAspect;
            return FVector2D(NewWidth, Target.Y);
        }
    }
    case EScalingMethod::Original:
        // if (Source.X > Target.X || Source.Y > Target.Y)
        //     return ScaleResolutionScreen(Source, Target, EScalingMethod::Fit);
        return Source;

    case EScalingMethod::Stretch:
        return Target;

    default:
        UE_LOG(LogTemp, Warning, TEXT("Invalid scaling method"));
        return FVector2D::ZeroVector;
    }
}