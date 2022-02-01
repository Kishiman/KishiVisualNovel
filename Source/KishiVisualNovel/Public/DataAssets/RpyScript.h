// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSprite.h"

#include "KishiDataAsset.h"
#include "Structs/DynamicObject.h"

#include "RpyScript.generated.h"

class IRpyInterpreter;
class RpyInstruction;
/**
 */
USTRUCT(BlueprintType)
struct FRpyImage
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Py")
	UPaperSprite* image=nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Py")
	FName name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Py")
	FString path;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Py")
	FName tag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Py")
	TArray<FName> attributes;
};
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
	URpyScript();
	UPROPERTY(BlueprintReadOnly)
	TArray<FRpyLine> rpyLines;
	UPROPERTY(EditAnywhere)
	TMap<FName,FRpyImage> images;

	FDynamicObject compileData;

	RpyInstruction *current = nullptr;
	RpyInstruction * init=nullptr;
	TArray<RpyInstruction *> instructions;
	TArray<RpyInstruction *> callStack;
	
	TMap<FName, RpyInstruction *> labels;

	UFUNCTION(BlueprintPure)
	TArray<FName> GetLabels() const;
	UFUNCTION(BlueprintPure)
	int GetInstructionsLength() const;

	UFUNCTION(BlueprintCallable)
	bool StartLabel(const TScriptInterface<IRpyInterpreter> &interpreter, FName label);

	UFUNCTION(BlueprintCallable)
	bool RunNext(const TScriptInterface<IRpyInterpreter> &interpreter);
	UFUNCTION(BlueprintCallable)
	bool RunInstruction(const TScriptInterface<IRpyInterpreter> &interpreter, int index);

	virtual void PostLoad() override;
	virtual void PostInitProperties() override;
	virtual void Serialize(FStructuredArchiveRecord Record) override;

	bool ImportRpyLines(FString text, uint8 TabSize);
	bool Parse();
	bool Compile();

private:
};
