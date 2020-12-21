#include "KishiVisualNovelScriptImporterFactory.h"
#include "CoreMinimal.h"
// #include "KishiVisualNovelScript.h"
// #include "Misc/Paths.h" 

UVisualNovelScriptImporterFactory::UVisualNovelScriptImporterFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer,UVisualNovelScript::StaticClass(),"rpy","Renpy Script Files",false,true),FKishiDataAssetReimportHandler(this)
{
};


FText UVisualNovelScriptImporterFactory::GetToolTip() const
{
	return NSLOCTEXT("KishiVisualNovel", "UVisualNovelScriptImporterFactoryDescription", "Py File imported from 'rpy' Ren'py Script files");
};

bool UVisualNovelScriptImporterFactory::FactoryUpdateString(UClass* InClass,UObject* Object, FString& content){
	UVisualNovelScript* KishiVisualNovelScript = (UVisualNovelScript*)(Object);
	TArray<FPyLine> PyLines=UPyFile::PYLinesFromString(content,TabSize);
	KishiVisualNovelScript->ScriptTreeFromPyFines(PyLines);
	return true;
};


