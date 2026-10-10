#include "KishiVisualNovelImporter.h"

#include "AssetTypeActions_RpyScript.h"
#include "AssetToolsModule.h"

#define LOCTEXT_NAMESPACE "FVisualNovelImporterModule"

void FVisualNovelImporterModule::StartupModule()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();
	RpyScriptAssetTypeActions = MakeShared<FAssetTypeActions_RpyScript>();
	AssetTools.RegisterAssetTypeActions(RpyScriptAssetTypeActions.ToSharedRef());
}

void FVisualNovelImporterModule::ShutdownModule()
{
	if (RpyScriptAssetTypeActions.IsValid() && FModuleManager::Get().IsModuleLoaded(TEXT("AssetTools")))
	{
		FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(RpyScriptAssetTypeActions.ToSharedRef());
	}
	RpyScriptAssetTypeActions.Reset();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FVisualNovelImporterModule, KishiVisualNovelImporter)
