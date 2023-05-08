#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PaperSprite.h"
#include "VectorUtils.generated.h"

UCLASS()
class KISHI_API UVectorUtils : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintPure)
  static FVector2D GetSize(UPaperSprite *Target);
  UFUNCTION(BlueprintPure)
  static FVector2D ClampPreserveRatio(FVector2D Target, FVector2D max);

protected:
private:
};