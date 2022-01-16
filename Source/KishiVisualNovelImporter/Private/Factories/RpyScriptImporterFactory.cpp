#include "Factories/RpyScriptImporterFactory.h"

URpyScriptImporterFactory::URpyScriptImporterFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer,URpyScript::StaticClass(),"py","Python Script Files",false,true,true),FKishiDataAssetReimportHandler(this)
{
};


FText URpyScriptImporterFactory::GetToolTip() const
{
	return NSLOCTEXT("KishiVisualNovel", "URpyScriptImporterFactoryDescription", "Py File imported from 'py' Python Script Files");
};

bool URpyScriptImporterFactory::FactoryUpdateString(UClass* InClass,UObject* Object, FString& content){
	URpyScript* rpyScript = (URpyScript*)(Object);
	rpyScript->PyLines=URpyScript::PYLinesFromString(content,TabSize);
	return true;
};


