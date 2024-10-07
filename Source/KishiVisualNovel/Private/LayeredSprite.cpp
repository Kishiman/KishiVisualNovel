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
bool ULayeredSprite::FindOnlyLayerByName(FName LayerName) const
{
	FSpriteLayer foundLayer;
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
	return true;
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

void ULayeredSprite::FindLayersByAttribute(FString Attribute, TArray<FSpriteLayer> &LayersFound, TArray<FSpriteLayer> &groupLayers) const
{
	TArray<FName> groupsFound;
	TArray<FName> namesFound;
	for (const auto &layer : layers)
	{
		if (Attribute.Contains(layer.name.ToString()))
		{
			namesFound.Add(layer.name);
			groupsFound.Add(layer.group);
		}
	}

	for (const auto &layer : layers)
	{
		if (namesFound.Contains(layer.name))
		{
			LayersFound.Add(layer);
		}
		else if (groupsFound.Contains(layer.group))
		{
			groupLayers.Add(layer);
		}
	}
}

bool ULayeredSprite::DisplayLayer(FName LayerName)
{
	FSpriteLayer foundLayer;
	TArray<FSpriteLayer> groupLayers;
	auto result = this->FindLayerByName(LayerName, foundLayer, groupLayers);
	if (!result)
		return result;
	foundLayer.displayed = true;
	for (auto layer : groupLayers)
	{
		layer.displayed = false;
	}
	return true;
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