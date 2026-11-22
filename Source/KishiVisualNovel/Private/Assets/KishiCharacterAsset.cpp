#include "Assets/KishiCharacterAsset.h"

FKishiSpeakerStyle UKishiCharacterAsset::GetSpeakerStyle_Implementation() const
{
	FKishiSpeakerStyle Style = SpeakerStyle;
	if (Style.DisplayName.IsEmpty())
	{
		Style.DisplayName = FText::FromString(GetName());
	}
	if (Style.Tag.IsNone())
	{
		Style.Tag = GetFName();
	}
	return Style;
}

bool UKishiCharacterAsset::ResolveVoiceLine_Implementation(const FKishiSpeechOptions &Speech, FKishiVoiceLine &OutLine) const
{
	const bool bExplicitId = !Speech.VoiceId.IsNone() && Speech.VoiceId != FName(TEXT("auto"));
	if (!bExplicitId && !bAutoVoice)
	{
		return false;
	}

	const FName VoiceId = USpeakerLibrary::GetVoiceId(Speech);
	if (VoiceId.IsNone())
	{
		return false;
	}

	OutLine = FKishiVoiceLine();
	OutLine.Volume = VoiceVolume;
	OutLine.bAdvanceOnVoiceEnd = bAdvanceOnVoiceEnd;

	if (USoundWave *const *Found = VoiceLines.Find(VoiceId))
	{
		if (*Found)
		{
			OutLine.Sound = *Found;
			OutLine.Path = (*Found)->GetPathName();
			return true;
		}
	}

	const FString Folder = VoiceFolder.IsEmpty() ? SpeakerStyle.Tag.ToString() : VoiceFolder;
	FString FoundPath;
	if (USoundWave *Sound = USpeakerLibrary::LoadVoiceSound(Folder, VoiceId, FoundPath))
	{
		OutLine.Sound = Sound;
		OutLine.Path = FoundPath;
		return true;
	}

	// A statement that names a clip explicitly and does not find it is worth
	// reporting; an automatic id that misses is the normal unvoiced line.
	if (bExplicitId)
	{
		UE_LOG(LogTemp, Warning, TEXT("[VNSpeaker] %s has no voice line '%s' (folder '%s')."),
					 *GetName(), *VoiceId.ToString(), *Folder);
	}
	return false;
}
