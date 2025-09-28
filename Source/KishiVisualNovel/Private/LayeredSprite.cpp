#include "LayeredSprite.h"

void ULayeredSprite::GetNameFromAssetName(FString AssetName, FName &OutLayerName, FName &OutGroupName)
{
	TArray<FString> strings;
	static const TCHAR *delimiters[] =
			{
					TEXT("_"),
			};
	AssetName.TrimStartAndEnd().ParseIntoArray(strings, delimiters, 1, true);
	if (strings.Num() < 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("LayeredSprite: AssetName '%s' is not valid for parsing"), *AssetName);
		OutGroupName = EName::NAME_None;
		OutLayerName = EName::NAME_None;
		return;
	}
	// remove prefix at start
	strings.RemoveAt(0);
	// remove Sprite at the end if exists
	strings.RemoveAt(strings.Num() - 1);
	switch (strings.Num())
	{
	case 0:
		OutGroupName = FName("Default");
		OutLayerName = FName("Default");
		break;
	case 1:
		OutGroupName = FName(*strings[0]);
		OutLayerName = FName("Default");
		break;
	default:
		OutGroupName = FName(*strings[0]);
		strings.RemoveAt(0);
		FString separator = TEXT(" ");
		FString joinedString = FString::Join(strings, *separator);
		OutLayerName = FName(joinedString);
		break;
	}
	return;
}
bool ULayeredSprite::FindOnlyLayerByName(FName LayerName, FSpriteLayer &FoundLayer) const
{
	auto layerString = LayerName.ToString();
	for (const auto &layer : layers)
	{
		auto groupString = layer.group.ToString();
		auto groupMatch = FString::Printf(TEXT("%s %s"), *groupString, *layerString);
		if (layer.name == LayerName || groupMatch == layer.name.ToString())
		{
			FoundLayer = layer;
			return true;
		}
	}
	return false;
}
bool ULayeredSprite::FindLayerByName(FName LayerName, FSpriteLayer &foundLayer, TArray<FSpriteLayer> &groupLayers) const
{
	auto layerString = LayerName.ToString();
	for (const auto &layer : layers)
	{
		auto groupString = layer.group.ToString();
		auto groupMatch = FString::Printf(TEXT("%s %s"), *groupString, *layerString);
		if (layer.name == LayerName || groupMatch == layer.name.ToString())
		{
			foundLayer = layer;
			break;
		}
	}
	if (foundLayer.name == NAME_None)
		return false;
	for (const auto &layer : layers)
	{
		if (layer.name != foundLayer.name && layer.group == foundLayer.group)
		{
			groupLayers.Add(layer);
		}
	}

	return true;
}

void ULayeredSprite::FindNamesByAttribute(FString Attribute, TArray<FName> &Names, TArray<FName> &InvertNames) const
{
	TSet<FName> groupsFound;
	for (const auto &layer : layers)
	{
		if (Attribute.Contains(layer.name.ToString()))
		{
			Names.Add(layer.name);
			groupsFound.Add(layer.group);
		}
	}

	for (const auto &layer : layers)
	{
		if (layer.displayed && !groupsFound.Contains(layer.group))
		{
			Names.Add(layer.name);
		}
		if (!Names.Contains(layer.name) && groupsFound.Contains(layer.group))
		{
			InvertNames.Add(layer.name);
		}
	}
	return;
}

void ULayeredSprite::FindLayersByGroup(FName GroupName, TArray<FSpriteLayer> &groupLayers) const
{
	for (const FSpriteLayer &layer : layers)
	{
		if (layer.group == GroupName)
		{
			groupLayers.Add(layer);
		}
	}
}
