// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "KishiDataAsset.h"
#include "C++/RpyInstruction.h"
#include "Interfaces/RpyInterpreter.h"

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

	UFUNCTION(BlueprintPure)
	TArray<FName> GetLabels()const;

    UFUNCTION(BlueprintCallable)
	bool StartLabel(const TScriptInterface<IRpyInterpreter>& interpreter,FName label);
	
	UFUNCTION(BlueprintCallable)
	bool RunNext(const TScriptInterface<IRpyInterpreter>& interpreter);

	static TArray<FRpyLine> PYLinesFromString(FString text, uint8 TabSize);

	RpyInstructionBase* current;
	TArray<RpyInstructionBase*> instructions;
	TMap<FName,RpyInstructionBase*> labels;

private:
};
