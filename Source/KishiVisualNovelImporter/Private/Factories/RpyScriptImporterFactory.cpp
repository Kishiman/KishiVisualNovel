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
	try
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
		return true;
	}
	catch (const std::exception &e)
	{
		UE_LOG(LogTemp, Error, TEXT("Caught exception: %s"), UTF8_TO_TCHAR(e.what()));
		return false;
	}
	catch (...)
	{
		UE_LOG(LogTemp, Error, TEXT("Caught unknown exception"));
		return false;
	}
};
EReimportResult::Type URpyScriptImporterFactory::Reimport(UObject *Obj)
{
	UE_LOG(LogTemp, Warning, TEXT("URpyScriptImporterFactory::Reimport"));
	return FKishiDataAssetReimportHandler::Reimport(Obj);
};
