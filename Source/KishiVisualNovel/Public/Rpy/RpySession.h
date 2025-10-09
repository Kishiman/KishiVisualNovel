// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Structs/DynamicObject.h"
#include "Rpy/RpyScript.h"
#include "Interfaces/RpyStateful.h"

#include "RpySession.generated.h"

class IRpyScriptInterpreter;
class RpyInstruction;
/**
 */

UCLASS(BlueprintType)
class KISHIVISUALNOVEL_API URpySessionState : public URpyState
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyInstructionSerialization currentInstruction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRpyInstructionSerialization> instructionsCallStack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSoftObjectPtr<URpyScript>> scripts;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDynamicObject runtimeData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpySceneManagerState sceneState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyShowManagerState showState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyStatementManagerState statementState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyAudioManagerState audioState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyChoiceManagerState choiceState;
	void PostInitProperties() override
	{
		Super::PostInitProperties();

		StateName = "RpySessionState";
	}
	virtual void PrintDebug() const override
	{
		Super::PrintDebug();

		UE_LOG(LogTemp, Warning, TEXT("Current Instruction: %d"), currentInstruction.index);
		UE_LOG(LogTemp, Warning, TEXT("Call Stack:"));
		for (const auto &instruction : instructionsCallStack)
		{
			UE_LOG(LogTemp, Warning, TEXT(" - %d"), instruction.index);
		}
	}
};

UCLASS(BlueprintType)
class KISHIVISUALNOVEL_API URpySession : public UObject, public IRpyStateful
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
	// get State Class
	virtual TSubclassOf<URpyState> GetStateClass_Implementation() override
	{
		return URpySessionState::StaticClass();
	}

	// Save state to State
	virtual URpyState *SaveToState_Implementation(UObject *Outer) override;
	// Load state from State
	virtual void LoadFromState_Implementation(const URpyState *State) override;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpySceneManagerState sceneState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyShowManagerState showState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyStatementManagerState statementState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyAudioManagerState audioState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyChoiceManagerState choiceState;
};
