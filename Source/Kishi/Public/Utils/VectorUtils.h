#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PaperSprite.h"
#include "VectorUtils.generated.h"

UENUM(BlueprintType)
enum class EScalingMethod : uint8
{
  Crop UMETA(DisplayName = "Crop"),
  Fit UMETA(DisplayName = "Fit"),
  Original UMETA(DisplayName = "Original"),
  Stretch UMETA(DisplayName = "Stretch")
};

UCLASS()
class KISHI_API UVectorUtils : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintPure)
  static FVector2D GetSize(UPaperSprite *Target);
  UFUNCTION(BlueprintPure)
  static FVector2D ScaleResolutionScreen(FVector2D Source, FVector2D Target, EScalingMethod Method);

protected:
private:
};