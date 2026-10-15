#pragma once

#include "CoreMinimal.h"
#include "PaperSprite.h"
#include "Sound/SoundWave.h"
#include "FileMediaSource.h"
#include "LevelSequence.h"
#include "VideoMediaSprite.h"
#include "LayeredSprite.h"

class URpyScript;

#include "RpyScriptTypes.generated.h"

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
	bool loop = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool tight = false;
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
