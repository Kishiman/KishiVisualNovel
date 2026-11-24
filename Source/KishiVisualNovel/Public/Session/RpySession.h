// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Structs/DynamicObject.h"
#include "Script/RpyScript.h"
#include "Session/RpyStateful.h"

#include "RpySession.generated.h"

class IRpyScriptInterpreter;
class URpyInstruction;
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScriptComplete);

UCLASS(BlueprintType, Blueprintable)
class KISHIVISUALNOVEL_API URpySession : public UObject, public IRpyStateful
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FOnScriptComplete OnScriptComplete;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	TScriptInterface<IRpyScriptInterpreter> interpreter;
	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
	TArray<URpyScript *> scripts;
	UPROPERTY(BlueprintReadWrite)
	FDynamicObject runtimeData;

	UPROPERTY(Transient)
	URpyInstruction *current = nullptr;
	/** The instruction whose Execute is running: current, or a child of a <together> block. None outside Execute. */
	UPROPERTY(Transient)
	URpyInstruction *executing = nullptr;
	UPROPERTY(Transient)
	TArray<URpyInstruction *> callStack;

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

	/** The instruction whose Execute is running, or None outside Execute. A manager that returns Wait keeps it, and reports completion for it. */
	UFUNCTION(BlueprintPure)
	URpyInstruction *GetExecutingInstruction() const { return executing; }

	/**
	 * Reports that Instruction, which returned Wait, has finished. If it is the current instruction, or the last child the current one
	 * waits on, the session advances through the interpreter's HandleInstructionComplete. A completion for any other instruction is
	 * ignored (it finished after the script moved on). None means the current instruction. Returns true if the completion was accepted.
	 */
	UFUNCTION(BlueprintCallable)
	bool CompleteInstruction(URpyInstruction *Instruction);

	/** The player asked to skip what the current instruction waits for: a pause ends; see URpyInstruction::Skip. */
	UFUNCTION(BlueprintCallable)
	void Skip();

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
