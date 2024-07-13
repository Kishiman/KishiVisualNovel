#include "Factories/PSDDataAssetImporterFactory.h"

UPSDDataAssetImporterFactory::UPSDDataAssetImporterFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer,ULayeredSprite::StaticClass(),"psd","Photoshop document",false,true,false),FKishiDataAssetReimportHandler(this)
{
};


FText UPSDDataAssetImporterFactory::GetToolTip() const
{
	return NSLOCTEXT("KishiVisualNovel", "UPSDDataAssetImporterFactoryDescription", "imported PSD File ");
};

bool UPSDDataAssetImporterFactory::FactoryUpdateBinary(UClass* InClass,UObject* Object,const uint8* data,uint8 size){
	ULayeredSprite* psdFile = (ULayeredSprite*)(Object);
	FString path=psdFile->AssetImportData->GetPathName();
	//psdFile->layers.Add(...)
	//TODO
	return true;
};


