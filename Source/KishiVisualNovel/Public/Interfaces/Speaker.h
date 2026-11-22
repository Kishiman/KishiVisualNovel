#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Sound/SoundWave.h"
#include "UObject/Interface.h"

#include "Speaker.generated.h"

class URichTextBlock;

/**
 * Identity and presentation a speaker reports to the statement manager.
 *
 * The two style handles point at rows of a rich-text style set, the same
 * FRichTextStyleRow format the statement manager already resolves <red> and
 * <blue> against (Content/Fonts/DT_RichTextStyle). Name and statement are
 * separate: a character usually styles its name, while the main character also
 * styles the body of a monologue.
 */
USTRUCT(BlueprintType)
struct KISHIVISUALNOVEL_API FKishiSpeakerStyle
{
	GENERATED_BODY()

	/** Name shown in the name box. An empty name falls back to the speaker object's name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speaker")
	FText DisplayName;

	/** Showable tag this speaker owns: viseme routing, hide, and the default voice folder. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speaker")
	FName Tag;

	/** Style row for the name box. Leave the table unset to keep the widget's own style. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speaker|Text", meta = (RowType = "RichTextStyleRow"))
	FDataTableRowHandle NameStyle;

	/** Style row for the statement body; a <red> span inside the line still wins for its own range. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speaker|Text", meta = (RowType = "RichTextStyleRow"))
	FDataTableRowHandle StatementStyle;

	/** Layered-sprite attributes applied when this speaker talks; feeds FKishiShowableOptions::Attributes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speaker|Text")
	FString DefaultAttributes;
};

/** What a single statement tells its speaker. */
USTRUCT(BlueprintType)
struct KISHIVISUALNOVEL_API FKishiSpeechOptions
{
	GENERATED_BODY()

	/** Voice id from the statement, such as voice="jhn_0012". None asks for the automatic id. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speech")
	FName VoiceId;

	/** Label the statement runs in; used to build an automatic voice id. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speech")
	FName LabelName;

	/** Index of the statement inside its label; used to build an automatic voice id. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speech")
	int32 LineIndex = INDEX_NONE;

	/** Attribute expression for this line, such as an emotion; empty means the speaker's default. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speech")
	FString Attributes;
};

/** A resolved voice clip for one statement, ready for the audio manager's "voice" channel. */
USTRUCT(BlueprintType)
struct KISHIVISUALNOVEL_API FKishiVoiceLine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice")
	USoundWave *Sound = nullptr;

	/** Package path the clip came from; empty when the speaker returned a direct reference. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice")
	FString Path;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice", meta = (ClampMin = "0.0"))
	float Volume = 1.0f;

	/** Advance the statement when the clip ends instead of waiting for the player. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice")
	bool bAdvanceOnVoiceEnd = false;
};

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API USpeaker : public UInterface
{
	GENERATED_BODY()
};

/**
 * An object that can be the speaker of a statement: a character asset, or an
 * actor or component in the world. A script references the speaker instead of
 * defining a character, so the name, the styles and the voice lines live with
 * the asset.
 */
class KISHIVISUALNOVEL_API ISpeaker
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ISpeaker|Required", meta = (ToolTip = "Required: report this speaker's display name, tag and rich-text style rows."))
	FKishiSpeakerStyle GetSpeakerStyle() const;
	virtual FKishiSpeakerStyle GetSpeakerStyle_Implementation() const
	{
		UE_LOG(LogTemp, Error, TEXT("Required ISpeaker::GetSpeakerStyle implementation missing on %s"), *GetNameSafe(_getUObject()));
		return {};
	}

	// Unlike the other required operations, this fallback logs nothing: a text-only
	// script has no voice lines, and that is not an error.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ISpeaker|Required", meta = (ToolTip = "Required: resolve the voice clip for this statement. Return false when the line has no voice; that is not an error."))
	bool ResolveVoiceLine(const FKishiSpeechOptions &Speech, FKishiVoiceLine &OutLine) const;
	virtual bool ResolveVoiceLine_Implementation(const FKishiSpeechOptions &Speech, FKishiVoiceLine &OutLine) const { return false; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ISpeaker|Hooks", meta = (ToolTip = "Optional: called when this speaker starts a statement. The default does nothing."))
	void OnSpeechStarted(const FKishiSpeechOptions &Speech);
	virtual void OnSpeechStarted_Implementation(const FKishiSpeechOptions &Speech) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ISpeaker|Hooks", meta = (ToolTip = "Optional: called when this speaker's statement is fully revealed or dismissed. The default does nothing."))
	void OnSpeechFinished();
	virtual void OnSpeechFinished_Implementation() {}
};

UCLASS()
class KISHIVISUALNOVEL_API USpeakerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** True when the object implements ISpeaker. */
	UFUNCTION(BlueprintPure, Category = "Speaker")
	static bool IsSpeaker(const UObject *Object);

	/** The object as a speaker, or an empty interface when it does not implement ISpeaker. */
	UFUNCTION(BlueprintPure, Category = "Speaker")
	static TScriptInterface<ISpeaker> AsSpeaker(UObject *Object);

	/**
	 * Load a speaker asset by script name, searching /Game and /KishiVisualNovel
	 * and their Characters and Speakers folders, the way images are resolved.
	 * Actor speakers are found in the world, not by name.
	 */
	UFUNCTION(BlueprintCallable, Category = "Speaker")
	static TScriptInterface<ISpeaker> LoadSpeakerByName(FName Name);

	/** The speaker's display name, falling back to its object name when the style leaves it empty. */
	UFUNCTION(BlueprintPure, Category = "Speaker")
	static FText GetDisplayText(const TScriptInterface<ISpeaker> &Speaker);

	/** The speaker's showable tag, falling back to FallbackName when the style leaves it unset. */
	UFUNCTION(BlueprintPure, Category = "Speaker")
	static FName GetTag(const TScriptInterface<ISpeaker> &Speaker, FName FallbackName);

	/** Dispatch ResolveVoiceLine through Execute_*, returning false when the speaker is invalid. */
	UFUNCTION(BlueprintCallable, Category = "Speaker")
	static bool ResolveVoiceLineForSpeech(const TScriptInterface<ISpeaker> &Speaker, const FKishiSpeechOptions &Speech, FKishiVoiceLine &OutLine);

	/** The voice id for a statement: the explicit id, or "<label>_<line index>" when it is unset. */
	UFUNCTION(BlueprintPure, Category = "Speaker")
	static FName GetVoiceId(const FKishiSpeechOptions &Speech);

	/**
	 * Find a voice clip by convention: Folder/VoiceId under /Game and
	 * /KishiVisualNovel, with and without a Voice folder. OutPath reports the
	 * package that was loaded.
	 */
	UFUNCTION(BlueprintCallable, Category = "Speaker")
	static USoundWave *LoadVoiceSound(const FString &Folder, FName VoiceId, FString &OutPath);

	/**
	 * Apply a style row to a rich text block as its default text style, so tags
	 * inside the text keep working. An unset handle clears the override.
	 */
	UFUNCTION(BlueprintCallable, Category = "Speaker")
	static bool ApplyRichTextStyleRow(URichTextBlock *Target, const FDataTableRowHandle &Row);

	/** Apply the speaker's name and statement style rows to the two blocks. Either block may be null. */
	UFUNCTION(BlueprintCallable, Category = "Speaker")
	static void ApplySpeakerStyle(URichTextBlock *NameBlock, URichTextBlock *StatementBlock, const FKishiSpeakerStyle &Style);
};
