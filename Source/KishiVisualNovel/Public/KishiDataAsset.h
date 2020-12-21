#pragma once



#include "EditorFramework/AssetImportData.h"
#include "Engine/DataAsset.h"  
#include "KishiDataAsset.generated.h"


UCLASS(abstract, Blueprintable)
class UKishiDataAsset:public UDataAsset
{
    GENERATED_BODY()
public:
#if WITH_EDITORONLY_DATA
	// Import data for this 
	
	UPROPERTY(VisibleAnywhere, Instanced, Category=ImportSettings)
	UAssetImportData* AssetImportData;
	// UObject interface
	virtual void PostInitProperties() override;
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
	virtual void Serialize(FArchive& Ar) override;
	// End of UObject interface
#endif
};
