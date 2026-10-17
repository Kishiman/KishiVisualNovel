#pragma once

#include "EditorFramework/AssetImportData.h"
#include "Engine/DataAsset.h"
#include "KishiDataAssetBase.generated.h"

UCLASS(abstract, Blueprintable)
class UKishiDataAssetBase : public UDataAsset
{
	GENERATED_BODY()
public:
	// Import data for this
	UPROPERTY(VisibleAnywhere, Instanced, Category = ImportSettings)
	UAssetImportData *AssetImportData;
#if WITH_EDITORONLY_DATA
	// UObject interface
	virtual void PostInitProperties() override;
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag> &OutTags) const override;
	virtual void Serialize(FArchive &Ar) override;
	// End of UObject interface
#endif
};
