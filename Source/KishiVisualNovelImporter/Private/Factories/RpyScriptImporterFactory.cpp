#include "Factories/RpyScriptImporterFactory.h"
#include <iostream>

URpyScriptImporterFactory::URpyScriptImporterFactory(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer, URpyScript::StaticClass(), "py", "Python Script Files", false, true, true), FKishiDataAssetReimportHandler(this){};

FText URpyScriptImporterFactory::GetToolTip() const
{
	return NSLOCTEXT("KishiVisualNovel", "URpyScriptImporterFactoryDescription", "Py File imported from 'py' Python Script Files");
};

bool URpyScriptImporterFactory::FactoryUpdateString(UClass *InClass, UObject *Object, FString &content)
{
	URpyScript *rpyScript = (URpyScript *)(Object);
	try
	{
		rpyScript->ImportRpyLines(content, TabSize);
		rpyScript->Parse();
		rpyScript->Compile();
		return true;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		return false;
	}
};
