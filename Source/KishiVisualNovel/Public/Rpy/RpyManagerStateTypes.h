#pragma once

#include "CoreMinimal.h"
#include "Rpy/RpyScriptTypes.h"

#include "RpyManagerStateTypes.generated.h"

USTRUCT(BlueprintType)
struct FRpyStatementManagerState
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString statement;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName with;
};

USTRUCT(BlueprintType)
struct FRpyChoiceManagerState
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> choices;
};

USTRUCT(BlueprintType)
struct FRpyAudioPlayState
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName channel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyAudio rpyAudio;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyAudioOptions options;
};
USTRUCT(BlueprintType)
struct FRpyAudioManagerState
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRpyAudioPlayState> audioStates;
};
