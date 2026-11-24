#pragma once

#include "CoreMinimal.h"
#include "Assets/KishiDataAssetBase.h"

#include "KishiShowableAsset.generated.h"

/**
 * Base class for the plugin's own showable assets, such as layered sprites and video media sprites.
 * Scripts can show any asset class that the showable registry in Project Settings maps to a showable
 * (Paper2D sprites need no wrapper); derive from this class for new asset types the plugin defines, so
 * one registry entry can cover them all.
 */
UCLASS(Abstract, BlueprintType, meta = (DisplayName = "Kishi Showable Asset"))
class KISHIVISUALNOVEL_API UKishiShowableAsset : public UKishiDataAssetBase
{
	GENERATED_BODY()
};
