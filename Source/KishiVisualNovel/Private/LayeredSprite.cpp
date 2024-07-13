#include "LayeredSprite.h"

FSpriteLayer ULayeredSprite::FindLayerByName(FName LayerName, TArray<FSpriteLayer> &groupLayers) const
{
	FSpriteLayer out;
	for (const auto &layer : layers)
	{
		if (layer.name == LayerName)
		{
			out = layer;
			break;
		}
	}
	if (out.name == NAME_None)
		return out;
	for (const auto &layer : layers)
	{
		if (layer.name != LayerName && layer.group == out.group)
		{
			groupLayers.Add(layer);
		}
	}

	return out;
}

TArray<FSpriteLayer> ULayeredSprite::FindLayersByGroup(FName GroupName) const
{
	TArray<FSpriteLayer> foundLayers;
	for (const FSpriteLayer &layer : layers)
	{
		if (layer.group == GroupName)
		{
			foundLayers.Add(layer);
		}
	}

	return foundLayers;
}