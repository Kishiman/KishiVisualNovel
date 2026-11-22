#include "Interfaces/Speaker.h"

#include "Components/RichTextBlock.h"
#include "Misc/PackageName.h"
#include "UObject/UObjectGlobals.h"

namespace
{
	/** Script names may contain spaces; package paths use underscores, as image lookup does. */
	FString ToPathSegment(FName Name)
	{
		return Name.ToString().Replace(TEXT(" "), TEXT("_"));
	}

	FString NormalizeFolder(const FString &Folder)
	{
		FString Result = Folder.Replace(TEXT("\\"), TEXT("/")).Replace(TEXT(" "), TEXT("_"));
		while (Result.EndsWith(TEXT("/")))
		{
			Result.LeftChopInline(1);
		}
		return Result;
	}

	USoundWave *LoadSoundAt(const FString &PackagePath, FString &OutPath)
	{
		if (!FPackageName::DoesPackageExist(PackagePath))
		{
			return nullptr;
		}
		if (USoundWave *Sound = Cast<USoundWave>(StaticLoadObject(USoundWave::StaticClass(), nullptr, *PackagePath)))
		{
			OutPath = PackagePath;
			return Sound;
		}
		return nullptr;
	}
}

bool USpeakerLibrary::IsSpeaker(const UObject *Object)
{
	return IsValid(Object) && Object->GetClass()->ImplementsInterface(USpeaker::StaticClass());
}

TScriptInterface<ISpeaker> USpeakerLibrary::AsSpeaker(UObject *Object)
{
	return IsSpeaker(Object) ? TScriptInterface<ISpeaker>(Object) : TScriptInterface<ISpeaker>();
}

TScriptInterface<ISpeaker> USpeakerLibrary::LoadSpeakerByName(FName Name)
{
	if (Name.IsNone())
	{
		return TScriptInterface<ISpeaker>();
	}

	const FString Segment = ToPathSegment(Name);
	const TArray<FString> SearchPaths = {
			TEXT("/Game/") + Segment,
			TEXT("/Game/Characters/") + Segment,
			TEXT("/Game/Speakers/") + Segment,
			TEXT("/KishiVisualNovel/") + Segment,
			TEXT("/KishiVisualNovel/Characters/") + Segment,
			TEXT("/KishiVisualNovel/Speakers/") + Segment,
	};

	for (const FString &SearchPath : SearchPaths)
	{
		if (!FPackageName::DoesPackageExist(SearchPath))
		{
			continue;
		}
		UObject *Loaded = StaticLoadObject(UObject::StaticClass(), nullptr, *SearchPath);
		if (IsSpeaker(Loaded))
		{
			return TScriptInterface<ISpeaker>(Loaded);
		}
		if (Loaded)
		{
			UE_LOG(LogTemp, Warning, TEXT("[VNSpeaker] '%s' resolved to %s, which does not implement ISpeaker."),
						 *Name.ToString(), *GetNameSafe(Loaded));
		}
	}
	return TScriptInterface<ISpeaker>();
}

FText USpeakerLibrary::GetDisplayText(const TScriptInterface<ISpeaker> &Speaker)
{
	UObject *Object = Speaker.GetObject();
	if (!IsSpeaker(Object))
	{
		return FText::GetEmpty();
	}
	const FKishiSpeakerStyle Style = ISpeaker::Execute_GetSpeakerStyle(Object);
	return Style.DisplayName.IsEmpty() ? FText::FromString(Object->GetName()) : Style.DisplayName;
}

FName USpeakerLibrary::GetTag(const TScriptInterface<ISpeaker> &Speaker, FName FallbackName)
{
	UObject *Object = Speaker.GetObject();
	if (!IsSpeaker(Object))
	{
		return FallbackName;
	}
	const FKishiSpeakerStyle Style = ISpeaker::Execute_GetSpeakerStyle(Object);
	return Style.Tag.IsNone() ? FallbackName : Style.Tag;
}

bool USpeakerLibrary::ResolveVoiceLineForSpeech(const TScriptInterface<ISpeaker> &Speaker, const FKishiSpeechOptions &Speech, FKishiVoiceLine &OutLine)
{
	OutLine = FKishiVoiceLine();
	UObject *Object = Speaker.GetObject();
	if (!IsSpeaker(Object))
	{
		return false;
	}
	if (!ISpeaker::Execute_ResolveVoiceLine(Object, Speech, OutLine))
	{
		return false;
	}
	if (!OutLine.Sound)
	{
		UE_LOG(LogTemp, Warning, TEXT("[VNSpeaker] %s reported a voice line with no sound for voice id '%s'."),
					 *GetNameSafe(Object), *GetVoiceId(Speech).ToString());
		return false;
	}
	return true;
}

FName USpeakerLibrary::GetVoiceId(const FKishiSpeechOptions &Speech)
{
	static const FName AutoVoice(TEXT("auto"));
	if (!Speech.VoiceId.IsNone() && Speech.VoiceId != AutoVoice)
	{
		return Speech.VoiceId;
	}
	if (Speech.LabelName.IsNone() || Speech.LineIndex < 0)
	{
		return NAME_None;
	}
	return FName(*FString::Printf(TEXT("%s_%04d"), *ToPathSegment(Speech.LabelName), Speech.LineIndex));
}

USoundWave *USpeakerLibrary::LoadVoiceSound(const FString &Folder, FName VoiceId, FString &OutPath)
{
	OutPath.Reset();
	if (VoiceId.IsNone())
	{
		return nullptr;
	}

	const FString Id = ToPathSegment(VoiceId);
	const FString Dir = NormalizeFolder(Folder);
	TArray<FString> SearchPaths;
	if (!Dir.IsEmpty())
	{
		if (Dir.StartsWith(TEXT("/")))
		{
			SearchPaths.Add(Dir + TEXT("/") + Id);
		}
		else
		{
			SearchPaths.Add(TEXT("/Game/") + Dir + TEXT("/") + Id);
			SearchPaths.Add(TEXT("/Game/Voice/") + Dir + TEXT("/") + Id);
			SearchPaths.Add(TEXT("/KishiVisualNovel/") + Dir + TEXT("/") + Id);
			SearchPaths.Add(TEXT("/KishiVisualNovel/Voice/") + Dir + TEXT("/") + Id);
		}
	}
	SearchPaths.Add(TEXT("/Game/") + Id);
	SearchPaths.Add(TEXT("/Game/Voice/") + Id);
	SearchPaths.Add(TEXT("/KishiVisualNovel/") + Id);
	SearchPaths.Add(TEXT("/KishiVisualNovel/Voice/") + Id);

	for (const FString &SearchPath : SearchPaths)
	{
		if (USoundWave *Sound = LoadSoundAt(SearchPath, OutPath))
		{
			return Sound;
		}
	}
	return nullptr;
}

bool USpeakerLibrary::ApplyRichTextStyleRow(URichTextBlock *Target, const FDataTableRowHandle &Row)
{
	if (!Target)
	{
		return false;
	}
	if (!Row.DataTable || Row.RowName.IsNone())
	{
		// No style row: keep whatever the widget was designed with.
		Target->ClearAllDefaultStyleOverrides();
		return false;
	}
	if (const FRichTextStyleRow *Found = Row.DataTable->FindRow<FRichTextStyleRow>(Row.RowName, TEXT("USpeakerLibrary::ApplyRichTextStyleRow"), false))
	{
		// The default style covers undecorated text only, so tags inside the line
		// still resolve against the block's own style set.
		Target->SetDefaultTextStyle(Found->TextStyle);
		return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("[VNSpeaker] Rich text style row '%s' is not in %s."),
				 *Row.RowName.ToString(), *GetNameSafe(Row.DataTable));
	Target->ClearAllDefaultStyleOverrides();
	return false;
}

void USpeakerLibrary::ApplySpeakerStyle(URichTextBlock *NameBlock, URichTextBlock *StatementBlock, const FKishiSpeakerStyle &Style)
{
	ApplyRichTextStyleRow(NameBlock, Style.NameStyle);
	ApplyRichTextStyleRow(StatementBlock, Style.StatementStyle);
}
