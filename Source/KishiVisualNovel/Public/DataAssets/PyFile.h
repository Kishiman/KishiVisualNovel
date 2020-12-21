// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KishiDataAsset.h"
#include "PyFile.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FPyLine
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Py")
	uint8 tabs=0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Py")
	int LineNumber=0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Py")
	FString line;
};

UCLASS()
class KISHIVISUALNOVEL_API UPyFile : public UKishiDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<FPyLine>	PyLines;
	static TArray<FPyLine> PYLinesFromString(FString text,uint8 TabSize);
};
