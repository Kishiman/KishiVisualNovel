// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Structs/DynamicObject.h"

#include "RpySession.generated.h"

class IRpyInterpreter;
class RpyInstruction;
/**
 */

UCLASS(BlueprintType)
class KISHIVISUALNOVEL_API URpySession : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	TScriptInterface<IRpyInterpreter> interpreter;
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	TArray<URpyScript *> Scripts;
	UPROPERTY(BlueprintReadWrite)
	FDynamicObject RuntimeData;

	RpyInstruction *current = nullptr;
	TArray<RpyInstruction *> callStack;

	UFUNCTION(BlueprintPure)
	TArray<FName> GetLabels() const;

	UFUNCTION(BlueprintCallable)
	bool StartLabel(FName label);
	UFUNCTION(BlueprintCallable)
	bool RunNext();
	UFUNCTION(BlueprintCallable)
	bool Run();

	// virtual void PostLoad() override;
	// virtual void PostInitProperties() override;
	// virtual void Serialize(FStructuredArchiveRecord Record) override;

private:
};
