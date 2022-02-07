#pragma once

#include "CoreMinimal.h"

#include "RichString.generated.h"

UCLASS(BlueprintType)
class URichString :public UObject
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure)
    FString GetFullString() const {
        return string;
        
    }
	UPROPERTY(BlueprintReadOnly,meta=(ExposeOnSpawn="true"))
    FString string;
};