#include "Factories/PyFileImporterFactory.h"

UPyFileImporterFactory::UPyFileImporterFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer,UPyFile::StaticClass(),"py","Python Script Files",false,true,true),FKishiDataAssetReimportHandler(this)
{
};


FText UPyFileImporterFactory::GetToolTip() const
{
	return NSLOCTEXT("KishiVisualNovel", "UPyFileImporterFactoryDescription", "Py File imported from 'py' Python Script Files");
};

bool UPyFileImporterFactory::FactoryUpdateString(UClass* InClass,UObject* Object, FString& content){
	UPyFile* pyFile = (UPyFile*)(Object);
	pyFile->PyLines=UPyFile::PYLinesFromString(content,TabSize);
	return true;
};


