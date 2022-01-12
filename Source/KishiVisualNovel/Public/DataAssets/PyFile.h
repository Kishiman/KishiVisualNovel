// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KishiDataAsset.h"
#include "KishiVisualNovelGameMode.h"
#include "Libraries/Tree/ArrayTree.h"
#include "PyFile.generated.h"

/**
 */
USTRUCT(BlueprintType)
struct FPyLine
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Py")
	uint8 tabs = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Py")
	int LineNumber = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Py")
	FString line;
};

class PyInstruction : public IArrayTree
{
public:
	FPyLine *pyLine;
	virtual void Execute(AKishiVisualNovelGameMode *gameMode) = 0;

protected:
	PyInstruction(FPyLine *_pyLine, PyInstruction *_parent) : pyLine(_pyLine), parent(_parent){};
	PyInstruction *parent;
	TArray<PyInstruction *> children;
};

UCLASS()
class KISHIVISUALNOVEL_API UPyFile : public UKishiDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<FPyLine> PyLines;
	static TArray<FPyLine> PYLinesFromString(FString text, uint8 TabSize);

private:
	TArray<PyInstruction *> instructions;
};
