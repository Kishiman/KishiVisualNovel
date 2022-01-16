// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KishiDataAsset.h"
#include "KVNGameMode.h"
#include "Libraries/Tree/ArrayTree.h"
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

class RpyInstruction : public IArrayTree
{
public:
	FRpyLine *rpyLine;
	virtual void Execute(AKVNGameMode *gameMode) = 0;

protected:
	RpyInstruction(FRpyLine *_rpyLine, RpyInstruction *_parent) : rpyLine(_rpyLine), parent(_parent){};
	RpyInstruction *parent;
	TArray<RpyInstruction *> children;
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
	TArray<RpyInstruction *> instructions;
};
