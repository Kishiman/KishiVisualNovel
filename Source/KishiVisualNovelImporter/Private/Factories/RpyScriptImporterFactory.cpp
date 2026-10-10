#include "Factories/RpyScriptImporterFactory.h"
#include <exception>

URpyScriptImporterFactory::URpyScriptImporterFactory(const FObjectInitializer &ObjectInitializer)
		: Super(ObjectInitializer, URpyScript::StaticClass(), "xml", "Visual Novel XML Scripts", false, true, true), FKishiDataAssetReimportHandler(this){};

FText URpyScriptImporterFactory::GetToolTip() const
{
	return NSLOCTEXT("KishiVisualNovel", "URpyScriptImporterFactoryDescription", "Versioned XML visual novel script");
};

bool URpyScriptImporterFactory::FactoryUpdateString(UClass *InClass, UObject *Object, FString &content)
{
	try
	{
		URpyScript *rpyScript = (URpyScript *)(Object);
		bool allGood = rpyScript->ImportXml(content);
		if (!allGood) return false;
		UE_LOG(LogTemp, Display, TEXT("URpyScript.ImportXml"));
		return true;
	}
	catch (const std::exception &e)
	{
		UE_LOG(LogTemp, Error, TEXT("Caught exception: %s"), e.what());
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
