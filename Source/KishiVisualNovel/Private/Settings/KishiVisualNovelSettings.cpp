#include "Settings/KishiVisualNovelSettings.h"

namespace
{
	FString NormalizeSearchPath(const FString &Path)
	{
		FString Result = Path.Replace(TEXT("\\"), TEXT("/")).Replace(TEXT(" "), TEXT("_"));
		Result.TrimStartAndEndInline();
		while (Result.EndsWith(TEXT("/")))
		{
			Result.LeftChopInline(1);
		}
		return Result;
	}
}

UKishiVisualNovelSettings::UKishiVisualNovelSettings()
{
	// The defaults keep the search order the plugin used before these settings existed:
	// each project root on its own, then its conventional subfolder.
	ImageSearchFolders = {TEXT("/Game"), TEXT("/Game/Images"), TEXT("/KishiVisualNovel"), TEXT("/KishiVisualNovel/Images")};
	AudioSearchFolders = {TEXT("/Game"), TEXT("/Game/Audio"), TEXT("/KishiVisualNovel"), TEXT("/KishiVisualNovel/Audio")};
	TransitionSearchFolders = {TEXT("/Game"), TEXT("/Game/Transitions"), TEXT("/KishiVisualNovel"), TEXT("/KishiVisualNovel/Transitions")};
	SpeakerSearchFolders = {TEXT("/Game"), TEXT("/Game/Characters"), TEXT("/Game/Speakers"), TEXT("/KishiVisualNovel"), TEXT("/KishiVisualNovel/Characters"), TEXT("/KishiVisualNovel/Speakers")};
	VoiceSearchFolders = {TEXT("/Game"), TEXT("/Game/Voice"), TEXT("/KishiVisualNovel"), TEXT("/KishiVisualNovel/Voice")};
}

const TArray<FString> &UKishiVisualNovelSettings::GetSearchFolders(EKishiAssetSearchKind Kind) const
{
	switch (Kind)
	{
	case EKishiAssetSearchKind::Audio:
		return AudioSearchFolders;
	case EKishiAssetSearchKind::Transition:
		return TransitionSearchFolders;
	case EKishiAssetSearchKind::Speaker:
		return SpeakerSearchFolders;
	case EKishiAssetSearchKind::Voice:
		return VoiceSearchFolders;
	case EKishiAssetSearchKind::Image:
	default:
		return ImageSearchFolders;
	}
}

TArray<FString> UKishiVisualNovelSettings::GetSearchPaths(EKishiAssetSearchKind Kind, const FString &RelativePath)
{
	TArray<FString> SearchPaths;
	FString Path = NormalizeSearchPath(RelativePath);
	if (Path.IsEmpty())
	{
		return SearchPaths;
	}
	if (Path.StartsWith(TEXT("/")))
	{
		SearchPaths.Add(Path);
		return SearchPaths;
	}

	for (const FString &Folder : GetDefault<UKishiVisualNovelSettings>()->GetSearchFolders(Kind))
	{
		const FString Root = NormalizeSearchPath(Folder);
		if (Root.IsEmpty())
		{
			continue;
		}
		SearchPaths.AddUnique(Root + TEXT("/") + Path);
	}
	return SearchPaths;
}

FName UKishiVisualNovelSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}
