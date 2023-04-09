#include "Factories/RpyScriptImporterFactory.h"
#include <iostream>

URpyScriptImporterFactory::URpyScriptImporterFactory(const FObjectInitializer &ObjectInitializer)
		: Super(ObjectInitializer, URpyScript::StaticClass(), "rpy", "Renpy Script Files", false, true, true), FKishiDataAssetReimportHandler(this){};

FText URpyScriptImporterFactory::GetToolTip() const
{
	return NSLOCTEXT("KishiVisualNovel", "URpyScriptImporterFactoryDescription", "Rpy File imported from 'rpy' Renpy Script Files");
};

bool URpyScriptImporterFactory::FactoryUpdateString(UClass *InClass, UObject *Object, FString &content)
{
	URpyScript *rpyScript = (URpyScript *)(Object);
	bool allGood = true;
	allGood = rpyScript->ImportRpyLines(content, TabSize);
	if (!allGood)
		return false;
	UE_LOG(LogTemp, Display, TEXT("URpyScript.ImportRpyLines"));
	allGood = rpyScript->Parse();
	if (!allGood)
		return false;
	UE_LOG(LogTemp, Display, TEXT("URpyScript.Parse"));
	allGood = rpyScript->Compile();
	if (!allGood)
		return false;
	UE_LOG(LogTemp, Display, TEXT("URpyScript.Compile"));
	rpyScript->LoadRpyData();
	UE_LOG(LogTemp, Display, TEXT("URpyScript.LoadRpyData"));
	return true;
};
EReimportResult::Type URpyScriptImporterFactory::Reimport(UObject *Obj)
{
	UE_LOG(LogTemp, Warning, TEXT("URpyScriptImporterFactory::Reimport"));
	return FKishiDataAssetReimportHandler::Reimport(Obj);
};
