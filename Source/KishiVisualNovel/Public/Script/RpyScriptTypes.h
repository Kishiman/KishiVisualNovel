#pragma once

#include "CoreMinimal.h"
#include "PaperSprite.h"
#include "Sound/SoundWave.h"
#include "FileMediaSource.h"
#include "LevelSequence.h"
#include "Showables/VideoMediaSprite.h"
#include "Showables/LayeredSprite.h"

class URpyScript;

#include "RpyScriptTypes.generated.h"

/**
 * What an instruction, or the manager operation it calls, did when it ran.
 * URpySession::Run advances on Continue, stops on Wait, and applies the Failed policy from
 * UKishiVisualNovelSettings on Failed.
 */
UENUM(BlueprintType)
enum class ERpyInstructionResult : uint8
{
	Continue UMETA(ToolTip = "Done. The session moves to the next instruction right away."),
	Wait UMETA(ToolTip = "Started, and finishes later. The session waits until the interpreter reports that the instruction completed (for example, when the player clicks through a line or picks a choice)."),
	Failed UMETA(ToolTip = "Could not run. Log why before returning this; the session then applies the Failed Instruction Policy from Project Settings > Plugins > Kishi Visual Novel."),
};

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
/** Legacy: the kind of asset an FRpyImage held before step 6. Read only to convert old scripts and saves; FRpyImage::asset replaces it. */
UENUM(BlueprintType)
enum struct ERpyImageType : uint8
{
	ENONE UMETA(DisplayName = "None"),
	ESPRITE UMETA(DisplayName = "Sprite"),
	ELAYERED_SPRITE UMETA(DisplayName = "Layered Sprite"),
	ELEVEL_SEQUENCE UMETA(DisplayName = "level Sequence"),
	EVIDEO_MEDIA_SPRITE UMETA(DisplayName = "Video Media Sprite"),
};

/**
 * An image a script shows: an asset and the tag it is shown under. The asset can be of any class the
 * showable registry in Project Settings maps to a showable (a Paper2D sprite, a layered sprite, a video
 * media sprite), or a level sequence for a scene. No asset means "nothing", as in "scene _".
 */
USTRUCT(BlueprintType)
struct KISHIVISUALNOVEL_API FRpyImage
{
	GENERATED_USTRUCT_BODY()
	/** The asset to show. Its class picks the showable through the registry in Project Settings. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Image")
	TSoftObjectPtr<UObject> asset;
	/** The name the script declared the image under. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Image")
	FName name;
	/** The tag the image is shown under; showing another image with the same tag replaces it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Image")
	FName tag;

	/** Returns the asset, loading it if needed, or null when there is none or it cannot be loaded. */
	UObject *LoadAsset() const { return asset.LoadSynchronous(); }

	/** Converts the legacy fields below into asset after loading old data. */
	void PostSerialize(const FArchive &Ar);

	// Legacy fields, kept under their old names so old scripts and saves load. PostSerialize moves them into
	// asset and clears them; do not read or write them.
	UPROPERTY()
	ERpyImageType type = ERpyImageType::ENONE;
	UPROPERTY()
	UPaperSprite *sprite = nullptr;
	UPROPERTY()
	ULayeredSprite *layeredSprite = nullptr;
	UPROPERTY()
	ULevelSequence *levelSequence = nullptr;
	UPROPERTY()
	UVideoMediaSprite *videoMediaSprite = nullptr;
	UPROPERTY()
	FString path;
};

template <>
struct TStructOpsTypeTraits<FRpyImage> : public TStructOpsTypeTraitsBase2<FRpyImage>
{
	enum
	{
		WithPostSerialize = true,
	};
};

