// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSprite.h"
#include "Sound/SoundWave.h"


#include "KishiDataAsset.h"
#include "Structs/DynamicObject.h"

#include "RpyScript.generated.h"

class IRpyInterpreter;
class RpyInstruction;
/**
 */
 
USTRUCT(BlueprintType)
struct FRpyCharacter
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName image;
};
USTRUCT(BlueprintType)
struct FRpyAudioOptions
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fadeIn=0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fadeOut=0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float volume=1;
};

USTRUCT(BlueprintType)
struct FRpyAudio
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundWave* audio=nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString path;
};

USTRUCT(BlueprintType)
struct FRpyImage
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSprite* image=nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString path;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName tag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> attributes;
};

USTRUCT(BlueprintType)
struct FRpyLine
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rpy")
	uint8 tabs = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rpy")
	int LineNumber = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rpy")
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
	UPROPERTY(EditAnywhere)
	TMap<FName,FRpyAudio> audios;

	UPROPERTY(EditAnywhere)
	TMap<FName,FRpyCharacter> characters;

	FDynamicObject compileData;

	// RpyInstruction *current = nullptr;
	RpyInstruction * init=nullptr;
	TArray<RpyInstruction *> instructions;
	// TArray<RpyInstruction *> callStack;
	
	TMap<FName, RpyInstruction *> labels;

	UFUNCTION(BlueprintPure)
	TArray<FName> GetLabels() const;
	UFUNCTION(BlueprintPure)
	int GetInstructionsLength() const;

	// UFUNCTION(BlueprintCallable)
	// bool StartLabel(const TScriptInterface<IRpyInterpreter> &interpreter, FName label);

	// UFUNCTION(BlueprintCallable)
	// bool RunNext(const TScriptInterface<IRpyInterpreter> &interpreter);
	// UFUNCTION(BlueprintCallable)
	// bool RunInstruction(const TScriptInterface<IRpyInterpreter> &interpreter, int index);

	virtual void PostLoad() override;
	virtual void PostInitProperties() override;
	virtual void Serialize(FStructuredArchiveRecord Record) override;

	bool ImportRpyLines(FString text, uint8 TabSize);
	bool Parse();
	bool Compile();

private:
};
