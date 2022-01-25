// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "KishiDataAsset.h"
#include "Structs/DynamicObject.h"

#include "RpyScript.generated.h"

class IRpyInterpreter;
class RpyInstruction;
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
	URpyScript();
	UPROPERTY(EditAnywhere)
	TArray<FRpyLine> rpyLines;

	FDynamicObject compileData;
	RpyInstruction *current = nullptr;
	TArray<RpyInstruction *> instructions;
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
