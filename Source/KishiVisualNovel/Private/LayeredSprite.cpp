#include "LayeredSprite.h"

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