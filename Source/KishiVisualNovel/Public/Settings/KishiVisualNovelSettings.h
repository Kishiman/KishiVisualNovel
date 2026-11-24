#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "KishiVisualNovelSettings.generated.h"

/** The kinds of asset a script names by a short path and the importer resolves by searching folders. */
UENUM(BlueprintType)
enum class EKishiAssetSearchKind : uint8
{
	Image,
	Audio,
	Transition,
	Speaker,
	Voice
};

/** What the session does when an instruction returns ERpyInstructionResult::Failed. */
UENUM(BlueprintType)
enum class ERpyFailedInstructionPolicy : uint8
{
	LogAndAdvance UMETA(DisplayName = "Log and Advance", ToolTip = "Log an error and move on to the next instruction, as if the failed one had finished."),
	LogAndStop UMETA(DisplayName = "Log and Stop", ToolTip = "Log an error and end the script there. The interpreter gets OnScriptComplete, as when a script reaches its end."),
};

/** Where a showable is presented: in UMG, or in the world as an actor or actor component. */
UENUM(BlueprintType)
enum class EKishiShowableSpace : uint8
{
	Widget UMETA(ToolTip = "A UMG widget, such as the ones a widget show manager places on its canvas."),
	World UMETA(ToolTip = "An actor or actor component, such as the ones a world-space show manager spawns."),
};

/** One row of the showable registry: the showable classes that present an asset class. */
USTRUCT(BlueprintType)
struct KISHIVISUALNOVEL_API FKishiShowableClassEntry
{
	GENERATED_BODY()

	/** The asset class this row handles. It also handles subclasses that have no row of their own. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Showable", meta = (AllowAbstract = "true"))
	TSoftClassPtr<UObject> AssetClass;

	/** The widget showable that presents this asset class. It must implement IShowable and accept the asset in SetShowableAsset. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Showable", meta = (MustImplement = "Showable"))
	TSoftClassPtr<UObject> WidgetShowableClass;

	/** The actor or actor component showable that presents this asset class in the world. It must implement IShowable and accept the asset in SetShowableAsset. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Showable", meta = (MustImplement = "Showable"))
	TSoftClassPtr<UObject> WorldShowableClass;
};

/**
 * Project-wide settings for KishiVisualNovel, under Project Settings > Plugins > Kishi Visual Novel.
 *
 * Scripts name assets by a short path, such as `Augustina` or `bg_club`. To resolve one, the plugin
 * tries `<folder>/<path>` for each search folder of that kind, in order, and the first asset of the
 * right type wins. A path that already starts with `/` is used as is.
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Kishi Visual Novel"))
class KISHIVISUALNOVEL_API UKishiVisualNovelSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UKishiVisualNovelSettings();

	UPROPERTY(config, EditAnywhere, Category = "Execution", meta = (ToolTip = "What the session does when an instruction fails, for example a show whose image cannot be displayed or a manager that is missing. Log and Advance keeps the script running; Log and Stop ends it."))
	ERpyFailedInstructionPolicy FailedInstructionPolicy = ERpyFailedInstructionPolicy::LogAndAdvance;

	UPROPERTY(config, EditAnywhere, Category = "Asset Search", meta = (ToolTip = "Folders searched, in order, for images named in scripts: sprites (with the `_Sprite` suffix), layered sprites, videos and level sequences. Use long package paths such as /Game/Images."))
	TArray<FString> ImageSearchFolders;

	UPROPERTY(config, EditAnywhere, Category = "Asset Search", meta = (ToolTip = "Folders searched, in order, for sound waves named by <play> and <queue> or declared as audio assets. Use long package paths such as /Game/Audio."))
	TArray<FString> AudioSearchFolders;

	UPROPERTY(config, EditAnywhere, Category = "Asset Search", meta = (ToolTip = "Folders searched, in order, for level sequences declared as transitions. Use long package paths such as /Game/Transitions."))
	TArray<FString> TransitionSearchFolders;

	UPROPERTY(config, EditAnywhere, Category = "Asset Search", meta = (ToolTip = "Folders searched, in order, for speaker assets (anything implementing ISpeaker) named by a statement. Use long package paths such as /Game/Characters."))
	TArray<FString> SpeakerSearchFolders;

	UPROPERTY(config, EditAnywhere, Category = "Asset Search", meta = (ToolTip = "Folders searched, in order, for voice lines. A speaker's voice folder is tried under each of these first, then the voice id on its own. Use long package paths such as /Game/Voice."))
	TArray<FString> VoiceSearchFolders;

	UPROPERTY(config, EditAnywhere, Category = "Showables", meta = (ToolTip = "Which showable presents each asset class that scripts show. A manager resolves an image's asset class against these rows, walking up the class hierarchy until a row has a showable for its space, so the most specific row wins. Managers can override rows for their own style."))
	TArray<FKishiShowableClassEntry> ShowableClasses;

	/**
	 * The showable class for AssetClass in Space: the first row in Entries whose asset class is AssetClass, or failing that its nearest
	 * parent class, and that has a showable for Space. Loads the showable class if needed. Returns null when no row matches.
	 */
	static TSubclassOf<UObject> ResolveShowableClass(const TArray<FKishiShowableClassEntry> &Entries, const UClass *AssetClass, EKishiShowableSpace Space);

	/** ResolveShowableClass against the project's ShowableClasses. */
	static TSubclassOf<UObject> ResolveShowableClass(const UClass *AssetClass, EKishiShowableSpace Space);

	/** The configured search folders for one kind of asset. */
	const TArray<FString> &GetSearchFolders(EKishiAssetSearchKind Kind) const;

	/**
	 * The package paths to try for a script path, in search order: `<folder>/<RelativePath>` for every
	 * search folder of the given kind. Spaces become underscores, as in script asset names. A path that
	 * starts with `/` is returned on its own.
	 */
	static TArray<FString> GetSearchPaths(EKishiAssetSearchKind Kind, const FString &RelativePath);

	virtual FName GetCategoryName() const override;
};
