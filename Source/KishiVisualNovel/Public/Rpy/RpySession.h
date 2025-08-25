// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Structs/DynamicObject.h"
#include "Rpy/RpyScript.h"

#include "RpySession.generated.h"

class IRpyScriptInterpreter;
class RpyInstruction;
/**
 */

USTRUCT(BlueprintType)
struct FRpyState
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyInstructionSerialization currentInstruction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRpyInstructionSerialization> instructionsCallStack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDynamicObject runtimeData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpySceneState sceneState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRpyShowState> showStates;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyStatementState statementState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRpyAudioState> audioStates;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyChoiceState choiceState;
};

UCLASS(BlueprintType)
class KISHIVISUALNOVEL_API URpySession : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	TScriptInterface<IRpyScriptInterpreter> interpreter;
	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
	TArray<URpyScript *> scripts;
	UPROPERTY(BlueprintReadWrite)
	FDynamicObject runtimeData;

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
	
	UFUNCTION(BlueprintCallable)
	FRpyState SaveState();
	UFUNCTION(BlueprintCallable)
	void LoadState(const FRpyState &State);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpySceneState sceneState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRpyShowState> showStates;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyStatementState statementState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRpyAudioState> audioStates;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyChoiceState choiceState;

};
