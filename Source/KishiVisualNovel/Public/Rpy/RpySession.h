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
	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
	TArray<URpyScript *> scripts;
	UPROPERTY(BlueprintReadWrite)
	FDynamicObject RuntimeData;

	RpyInstruction *current = nullptr;
	TArray<RpyInstruction *> callStack;

	virtual void PostInitProperties() override;

	UFUNCTION(BlueprintCallable)
	void AddScript(URpyScript *script);
	UFUNCTION(BlueprintCallable)
	void ClearScript(URpyScript *script);

	UFUNCTION(BlueprintPure)
	TArray<FName> GetLabels() const;

	UFUNCTION(BlueprintCallable)
	bool StartLabel(FName label);
	UFUNCTION(BlueprintCallable)
	bool RunNext();
	UFUNCTION(BlueprintCallable)
	bool Run();
	UFUNCTION(BlueprintPure)
	EInstructionRunTimeType GetRunTimeType() const;

	UFUNCTION(BlueprintCallable)
	bool OnChoice(int index);

private:
};
