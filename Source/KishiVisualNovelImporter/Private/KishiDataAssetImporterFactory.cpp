#include "KishiDataAssetImporterFactory.h"
#include "DataAssets/TextFile.h"
#include "Misc/Paths.h"

UKishiDataAssetImporterFactory::UKishiDataAssetImporterFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UE_LOG(LogTemp, Warning, TEXT("UKishiDataAssetImporterFactory default constructor called, this should only happen Once!"));
}
UKishiDataAssetImporterFactory::UKishiDataAssetImporterFactory(const FObjectInitializer& ObjectInitializer,TSubclassOf<UObject> T,char const* ext,char const* desc,bool bnew,bool bedit,bool btext)
	: Super(ObjectInitializer)
{
	bCreateNew = bnew;
    bEditAfterNew=bedit;
	SupportedClass = T;
	bEditorImport = true;
	bText = btext;
	extension=ext;
	description=desc;
	Formats.Add(extension+";"+description);
}

FText UKishiDataAssetImporterFactory::GetToolTip() const
{
	return NSLOCTEXT("KishiVisualNovel", "UKishiDataAssetImporterFactoryDescription", "Default Kishi Data Asset Importer");
}
bool UKishiDataAssetImporterFactory::FactoryCanImport(const FString& Filename)
{
	return FPaths::GetExtension(Filename)==extension;
}
UObject* UKishiDataAssetImporterFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
		UObject* object = NewObject<UObject>(InParent,InClass, InName, Flags|RF_Transactional);
		return object;
}
UObject* UKishiDataAssetImporterFactory::FactoryCreateText(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const TCHAR*& Buffer, const TCHAR* BufferEnd, FFeedbackContext* Warn) 
{
	UObject* object;
	bool validImport=true;
	FString content(BufferEnd - Buffer, Buffer);
	// if(bNew){
	object=FactoryCreateNew(InClass,InParent,InName,Flags,Context,Warn);
	validImport=FactoryUpdateString(InClass,object,content);
	// }else{
	// 	object=FactoryCreateString(InClass,content);
	// 	validImport=(object!=nullptr);
	// }
	if(validImport){
		// UKishiDataAsset* kishiDataAsset = Cast<UKishiDataAsset>(object);
		UKishiDataAsset* kishiDataAsset =(UKishiDataAsset*)(object);
		UE_LOG(LogTemp, Display, TEXT("Import Successful"));
		const FString FactoryCurrentFilename = UFactory::GetCurrentFilename();
		kishiDataAsset->AssetImportData->Update(FactoryCurrentFilename);
		return kishiDataAsset;
	}else{
		UE_LOG(LogTemp, Error, TEXT("Import Failed!"));
		return nullptr;
	}
	return nullptr;
}
	UObject* UKishiDataAssetImporterFactory::FactoryCreateBinary(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd, FFeedbackContext* Warn)
{
	UObject* object;
	bool validImport=true;
	// if(bNew){
	object=FactoryCreateNew(InClass,InParent,InName,Flags,Context,Warn);
	validImport=FactoryUpdateBinary(InClass,object,Buffer,BufferEnd-Buffer);
	// }else{
	// 	object=FactoryCreateString(InClass,content);
	// 	validImport=(object!=nullptr);
	// }
	if(validImport){
		// UKishiDataAsset* kishiDataAsset = Cast<UKishiDataAsset>(object);
		UKishiDataAsset* kishiDataAsset =(UKishiDataAsset*)(object);
		UE_LOG(LogTemp, Display, TEXT("Import Successful"));
		const FString FactoryCurrentFilename = UFactory::GetCurrentFilename();
		kishiDataAsset->AssetImportData->Update(FactoryCurrentFilename);
		return kishiDataAsset;
	}else{
		UE_LOG(LogTemp, Error, TEXT("Import Failed!"));
		return nullptr;
	}
	return nullptr;
}


