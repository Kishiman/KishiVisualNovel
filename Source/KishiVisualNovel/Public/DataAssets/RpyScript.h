// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "KishiDataAsset.h"
#include "Interfaces/C++/RpyInstruction.h"

#include "RpyScript.generated.h"

/**
 */
USTRUCT(BlueprintType)
struct FRpyLine
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Py")
	uint8 tabs = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Py")
	int LineNumber = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Py")
	FString line;
};

UCLASS()
class KISHIVISUALNOVEL_API URpyScript : public UKishiDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<FRpyLine> PyLines;
	static TArray<FRpyLine> PYLinesFromString(FString text, uint8 TabSize);

private:
	TArray<TScriptInterface<IRpyInstruction>> instructions;
};
