// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSprite.h"
#include "Sound/SoundWave.h"
#include "FileMediaSource.h"
#include "LevelSequence.h"
#include "VideoMediaSprite.h"

#include "KishiDataAsset.h"
#include "LayeredSprite.h"
#include "Structs/DynamicObject.h"

#include "RpyScript.generated.h"

class IRpyScriptInterpreter;
class RpyInstruction;
/**
 */

UENUM(BlueprintType)
enum class EInstructionRunTimeType : uint8
{
	NONE,
	SAY,
	MENU,
	SCENE,
	STOP,
	PLAY,
	PAUSE,
	SHOW,
	HIDE,
};

UENUM(BlueprintType)
enum class ELayerType : uint8
{
	NONE,
	MASTER,
	TRANSIENT,
	UNDERLAY,
	OVERLAY,
};

TMap<FString, ELayerType> const stringToELayer = {
		{"master", ELayerType::MASTER},
		{"transient", ELayerType::TRANSIENT},
		{"underlay", ELayerType::UNDERLAY},
		{"overlay", ELayerType::OVERLAY},
};

UENUM(BlueprintType)
enum class EPositionType : uint8
{
	NONE,
	VECTOR,
	NAME,
};

UENUM(BlueprintType)
enum class ERPYTransitionType : uint8
{
	NONE,
	BLUR,
	DISSOLVE,
	EASE,
	FADE,
	MOVE,
	SHAKE,
	WIPE,
	ZOOM,
	LEVEL_SEQUENCE,
};

UENUM(BlueprintType)
enum class ERPYTransitionDirection : uint8
{
	NONE,
	LEFT,
	RIGHT,
	UP,
	DOWN,
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
struct FRpyTransition
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERPYTransitionType type = ERPYTransitionType::NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULevelSequence *levelSequence = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString path;
};
USTRUCT(BlueprintType)
struct FRpySceneOptions
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName layer = FName("master");
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString attribute;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERPYTransitionType with = ERPYTransitionType::NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyTransition transition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERPYTransitionDirection direction = ERPYTransitionDirection::NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPositionType positionType = EPositionType::NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName positionName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector position = FVector(0);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float transitionTime = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int zorder = 0;
};

UENUM(BlueprintType)
enum struct ERpyImageType : uint8
{
	ENONE UMETA(DisplayName = "None"),
	ESPRITE UMETA(DisplayName = "Sprite"),
	ELAYERED_SPRITE UMETA(DisplayName = "Layered Sprite"),
	ELEVEL_SEQUENCE UMETA(DisplayName = "level Sequence"),
	EVIDEO_MEDIA_SPRITE UMETA(DisplayName = "Video Media Sprite"),
};

USTRUCT(BlueprintType)
struct FRpyImage
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERpyImageType type = ERpyImageType::ENONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSprite *sprite = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULayeredSprite *layeredSprite = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULevelSequence *levelSequence = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UVideoMediaSprite *videoMediaSprite = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString path;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName tag;
	static FRpyImage MakeLayeredImage(FName name, FString path);
};

USTRUCT(BlueprintType)
struct FRpyLine
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rpy")
	uint8 tabs = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rpy")
	int lineNumber = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rpy")
	FString line;
};

USTRUCT(BlueprintType)
struct FRpySceneManagerState
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyImage rpyImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpySceneOptions options;
};

USTRUCT(BlueprintType)
struct FRpyShowImageState
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyImage rpyImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpySceneOptions options;
};

USTRUCT(BlueprintType)
struct FRpyShowManagerState
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRpyShowImageState> imageStates;
};

USTRUCT(BlueprintType)
struct FRpyStatementManagerState
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString statement;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName with;
};

USTRUCT(BlueprintType)
struct FRpyChoiceManagerState
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> choices;
};

USTRUCT(BlueprintType)
struct FRpyAudioPlayState
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName channel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyAudio rpyAudio;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyAudioOptions options;
};
USTRUCT(BlueprintType)
struct FRpyAudioManagerState
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRpyAudioPlayState> audioStates;
};

USTRUCT(BlueprintType)
struct FRpyInstructionSerialization
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TSoftObjectPtr<URpyScript> script;
	UPROPERTY()
	int index;
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
	TMap<FName, FRpyTransition> transitions;

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
	bool AddDefaultTransition(FName name, FString path);
	bool AddDefaultImage(FName name, FString path);
	bool AddDefaultAudio(FName name, FString path);

	bool ImportRpyLines(FString text, uint8 TabSize);
	bool Parse();
	bool Compile();
	void LoadRpyData();

	static FRpyInstructionSerialization SerializeInstruction(RpyInstruction *instruction);
	static RpyInstruction *DeserializeInstruction(const FRpyInstructionSerialization &data);

#if WITH_EDITORONLY_DATA
	bool IsAssetUnderProjectContent();
	bool IsAssetUnderPluginContent();
#endif

private:
};
