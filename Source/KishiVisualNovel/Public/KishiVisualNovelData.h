#pragma once

#include "CoreMinimal.h"

#include "KishiVisualNovelData.generated.h"
UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE(FVNEvent);

USTRUCT(BlueprintType)
struct FVisualNovelParameterCollection
{
    GENERATED_BODY()

public:
    FVisualNovelParameterCollection(int dummy=0){};
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flags")
    TSet<FName> NameFlags;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Maps")
    TMap<FName,bool> Booleans;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Maps")
    TMap<FName,FName> Names;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Maps")
    TMap<FName,int> Integers;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Maps")
    TMap<FName,float> Floats;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Maps")
    TMap<FName,FVector> Vectors;
};