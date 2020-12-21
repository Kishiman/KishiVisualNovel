#include "Factories/TextFileImporterFactory.h"


UTextFileImporterFactory::UTextFileImporterFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer,UTextFile::StaticClass(),"txt","Plain Text File",false,true),FKishiDataAssetReimportHandler(this)
{
};


FText UTextFileImporterFactory::GetToolTip() const
{
	return NSLOCTEXT("KishiVisualNovel", "UTextFileImporterFactoryDescription", "Text Files imported from Plain 'txt' files");
};

bool UTextFileImporterFactory::FactoryUpdateString(UClass* InClass,UObject* Object, FString& content){
	UTextFile* textFile = (UTextFile*)(Object);
	textFile->text=FText::FromString(content);
	return true;
};


