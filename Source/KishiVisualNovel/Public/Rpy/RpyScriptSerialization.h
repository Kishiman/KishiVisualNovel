#pragma once

#include "CoreMinimal.h"

class URpyScript;

#include "RpyScriptSerialization.generated.h"

USTRUCT(BlueprintType)
struct FRpyInstructionSerialization
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TSoftObjectPtr<URpyScript> script;
	UPROPERTY()
	int index = -1;
	UPROPERTY()
	FName instructionId;
};
