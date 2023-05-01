// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSprite.h"
#include "Sound/SoundWave.h"
#include "FileMediaSource.h"

#include "KishiDataAsset.h"
#include "Structs/DynamicObject.h"

#include "RpyScript.generated.h"

class IRpyInterpreter;
class RpyInstruction;
/**
 */

UENUM(BlueprintType)
enum class EInstructionRunTimeType : uint8
{
	NONE,
	SAY,
	SCENE,
	STOP,
	PLAY,
	PAUSE,
	SHOW,
	HIDE,
};
DECLARE_DYNAMIC_DELEGATE(FInstructionCompleteDelegate);

UENUM(BlueprintType)
enum struct ESchrodBool : uint8
{
	ETrue UMETA(DisplayName = "True"),
	EFalse UMETA(DisplayName = "False"),
	ENone UMETA(DisplayName = "None"),
};

USTRUCT(BlueprintType)
struct FRpyCharacter
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName image;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName voice;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor color;
};
USTRUCT(BlueprintType)
struct FRpyAudioOptions
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fadeIn = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fadeOut = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float volume = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESchrodBool loop = ESchrodBool::ENone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESchrodBool tight = ESchrodBool::ENone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool synchroStart = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool if_changed = false;
};

USTRUCT(BlueprintType)
struct FRpyAudio
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundWave *audio = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString path;
};

USTRUCT(BlueprintType)
struct FRpyMovie
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UFileMediaSource *movie = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString path;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UFileMediaSource *maskMovie = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString maskPath;
};

USTRUCT(BlueprintType)
struct FRpyImageOptions
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName layer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName with;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fadeTime = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float desolveTime = 0;
};
USTRUCT(BlueprintType)
struct FRpyImage
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSprite *image = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString path;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName tag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> attributes;
	static FRpyImage Make(FString name, FString path);
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FRpyLine> rpyLines;
	UPROPERTY(EditAnywhere)
	TMap<FName, FRpyImage> images;
	UPROPERTY(EditAnywhere)
	TMap<FName, FRpyAudio> audios;

	UPROPERTY(EditAnywhere)
	TMap<FName, FRpyCharacter> characters;

	UPROPERTY(BlueprintReadOnly)
	FDynamicObject compileData;

	// RpyInstruction *current = nullptr;
	RpyInstruction *init = nullptr;
	TArray<RpyInstruction *> instructions;
	// TArray<RpyInstruction *> callStack;

	TMap<FName, RpyInstruction *> labels;

	UFUNCTION(BlueprintPure)
	TArray<FName> GetLabels() const;
	UFUNCTION(BlueprintPure)
	int GetInstructionsLength() const;

	virtual void PostLoad() override;
	virtual void PostInitProperties() override;
	virtual void Serialize(FStructuredArchiveRecord Record) override;
	bool IsAssetUnderProjectContent();
	bool IsAssetUnderPluginContent();
	bool AddDefaultImage(FString name);
	bool AddDefaultAudio(FString param);


	bool ImportRpyLines(FString text, uint8 TabSize);
	bool Parse();
	bool Compile();
	void LoadRpyData();

private:
};
