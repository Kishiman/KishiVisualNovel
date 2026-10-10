#pragma once

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"

class URpyScript;

class FRpyScriptEditor : public FAssetEditorToolkit
{
public:
	void InitRpyScriptEditor(EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, URpyScript* Script);

	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

private:
	void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	TSharedRef<SDockTab> SpawnInstructionTreeTab(const FSpawnTabArgs& Args);

	TWeakObjectPtr<URpyScript> ScriptAsset;
	static const FName InstructionTreeTabId;
};
