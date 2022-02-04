// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Structs/DynamicObject.h"

#include "RpySession.generated.h"

class IRpyInterpreter;
class RpyInstruction;
/**
 */

UCLASS()
class KISHIVISUALNOVEL_API URpySession : public UObject
{
	GENERATED_BODY()
public:
	URpySession();

	UPROPERTY(EditAnywhere)
	FDynamicObject RuntimeData;
	UPROPERTY(EditAnywhere)
	TArray<URpyScript *> Scripts;

	RpyInstruction *current = nullptr;
	TArray<RpyInstruction *> callStack;

	UFUNCTION(BlueprintPure)
	TArray<FName> GetLabels() const;

	UFUNCTION(BlueprintCallable)
	bool StartLabel(const TScriptInterface<IRpyInterpreter> &interpreter, FName label);
	UFUNCTION(BlueprintCallable)
	bool RunNext(const TScriptInterface<IRpyInterpreter> &interpreter);

	// virtual void PostLoad() override;
	// virtual void PostInitProperties() override;
	// virtual void Serialize(FStructuredArchiveRecord Record) override;

private:
};
