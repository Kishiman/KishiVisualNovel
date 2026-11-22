#pragma once

#include "CoreMinimal.h"
#include "Assets/KishiDataAssetBase.h"
#include "Interfaces/Speaker.h"

#include "KishiCharacterAsset.generated.h"

/**
 * A character a script can speak as: the display name, the showable tag, the
 * rich-text style rows and the voice lines in one asset. A statement
 * references the asset (DA_John) instead of defining a character inline.
 *
 * Blueprint subclasses can override GetSpeakerStyle to make the name depend on
 * game state, or ResolveVoiceLine to use their own voice lookup.
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "VN Character"))
class KISHIVISUALNOVEL_API UKishiCharacterAsset : public UKishiDataAssetBase, public ISpeaker
{
	GENERATED_BODY()

public:
	/** Display name, showable tag and the two rich-text style rows. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FKishiSpeakerStyle SpeakerStyle;

	/** Voice clips by voice id, checked before the folder convention. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Voice")
	TMap<FName, USoundWave *> VoiceLines;

	/**
	 * Folder the voice clips are looked up in, relative to /Game or
	 * /KishiVisualNovel, or an absolute package path. Empty falls back to the
	 * speaker's tag.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Voice")
	FString VoiceFolder;

	/** Resolve a voice line even when the statement names no voice id, from the label and line index. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Voice")
	bool bAutoVoice = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Voice", meta = (ClampMin = "0.0"))
	float VoiceVolume = 1.0f;

	/** Advance the statement when the voice clip ends instead of waiting for the player. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Voice")
	bool bAdvanceOnVoiceEnd = false;

	/** An unset display name or tag falls back to the asset's own name. */
	virtual FKishiSpeakerStyle GetSpeakerStyle_Implementation() const override;
	virtual bool ResolveVoiceLine_Implementation(const FKishiSpeechOptions &Speech, FKishiVoiceLine &OutLine) const override;
};
