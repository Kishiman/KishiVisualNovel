#include "LayeredSprite.h"

FName ULayeredSprite::GetNameFromAssetName(FString AssetName)
{
	TArray<FString> strings;
	static const TCHAR *delimiters[] =
			{
					TEXT("_"),
			};
	AssetName.TrimStartAndEnd().ParseIntoArray(strings, delimiters, 1, true);
	strings.RemoveAt(0);
	strings.RemoveAt(strings.Num() - 1);
	FString separator = TEXT(" ");
	FString joinedString = FString::Join(strings, *separator);
	return FName(joinedString);
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
		if (!Names.Contains(layer.name) && groupsFound.Contains(layer.group))
		{
			InvertNames.Add(layer.name);
		}
	}
}

void ULayeredSprite::FindNamesToDisplayByAttribute(FString Attribute, TArray<FName> &NamesToShow, TArray<FName> &NamesToHide)
{
	TArray<FName> Names;
	TArray<FName> InvertNames;
	FindNamesByAttribute(Attribute, Names, InvertNames);
	for (const auto &layer : layers)
	{
		if (Names.Contains(layer.name) && !layer.displayed)
		{
			NamesToShow.Add(layer.name);
		}
		else if (InvertNames.Contains(layer.name) && layer.displayed)
		{
			NamesToHide.Add(layer.name);
		}
	}
	return;
}

void ULayeredSprite::SetLayerDisplay(FName LayerName, bool Value)
{
	for (auto &layer : layers)
	{
		if (layer.name == LayerName)
		{
			layer.displayed = Value;
			return;
		}
	}
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

void ULayeredSprite::DisplayLayersByAttribute(FString Attribute)
{
	TSet<FName> groupsFound;
	TArray<FName> namesFound;
	for (const auto &layer : layers)
	{
		if (Attribute.Contains(layer.name.ToString()))
		{
			namesFound.Add(layer.name);
			groupsFound.Add(layer.group);
		}
	}

	for (auto &layer : layers)
	{
		if (namesFound.Contains(layer.name))
		{
			layer.displayed = true;
		}
		else if (groupsFound.Contains(layer.group))
		{
			layer.displayed = false;
		}
	}
}