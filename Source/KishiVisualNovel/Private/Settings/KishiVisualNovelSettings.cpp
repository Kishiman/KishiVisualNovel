#include "Settings/KishiVisualNovelSettings.h"

#include "PaperSprite.h"
#include "Showables/KishiLayeredSpriteWidget.h"
#include "Showables/KishiSpriteWidget.h"
#include "Showables/LayeredSprite.h"
#include "Showables/VideoMediaSprite.h"

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

	// The showables the show manager Base used before the registry existed.
	auto AddShowableClass = [this](UClass *AssetClass, UClass *WidgetShowableClass)
	{
		FKishiShowableClassEntry &Entry = ShowableClasses.AddDefaulted_GetRef();
		Entry.AssetClass = AssetClass;
		Entry.WidgetShowableClass = WidgetShowableClass;
	};
	AddShowableClass(UPaperSprite::StaticClass(), UKishiSpriteWidget::StaticClass());
	AddShowableClass(UVideoMediaSprite::StaticClass(), UKishiSpriteWidget::StaticClass());
	AddShowableClass(ULayeredSprite::StaticClass(), UKishiLayeredSpriteWidget::StaticClass());
}

TSubclassOf<UObject> UKishiVisualNovelSettings::ResolveShowableClass(const TArray<FKishiShowableClassEntry> &Entries, const UClass *AssetClass, EKishiShowableSpace Space)
{
	for (const UClass *Class = AssetClass; Class; Class = Class->GetSuperClass())
	{
		const FSoftObjectPath ClassPath(Class);
		for (const FKishiShowableClassEntry &Entry : Entries)
		{
			if (Entry.AssetClass.ToSoftObjectPath() != ClassPath)
				continue;
			const TSoftClassPtr<UObject> &ShowableClass = Space == EKishiShowableSpace::Widget ? Entry.WidgetShowableClass : Entry.WorldShowableClass;
			if (ShowableClass.IsNull())
				continue;
			if (UClass *Loaded = ShowableClass.LoadSynchronous())
				return Loaded;
			UE_LOG(LogTemp, Error, TEXT("The showable class %s registered for %s could not be loaded."), *ShowableClass.ToString(), *Class->GetName());
		}
	}
	return nullptr;
}

TSubclassOf<UObject> UKishiVisualNovelSettings::ResolveShowableClass(const UClass *AssetClass, EKishiShowableSpace Space)
{
	return ResolveShowableClass(GetDefault<UKishiVisualNovelSettings>()->ShowableClasses, AssetClass, Space);
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
