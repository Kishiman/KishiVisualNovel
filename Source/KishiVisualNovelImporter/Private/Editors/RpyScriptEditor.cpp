#include "Editors/RpyScriptEditor.h"

#include "Editors/SRpyInstructionTree.h"
#include "Rpy/RpyScript.h"
#include "Widgets/Docking/SDockTab.h"

const FName FRpyScriptEditor::InstructionTreeTabId(TEXT("KishiRpyInstructionTree"));

void FRpyScriptEditor::InitRpyScriptEditor(EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, URpyScript* Script)
{
	ScriptAsset = Script;
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("KishiRpyScriptEditor_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(1.0f)
				->AddTab(InstructionTreeTabId, ETabState::OpenedTab)
			)
		);

	const TArray<UObject*> EditedObjects = { Script };
	InitAssetEditor(Mode, InitToolkitHost, FName("KishiRpyScriptEditor"), Layout, true, true, EditedObjects);
	RegenerateMenusAndToolbars();
}

FName FRpyScriptEditor::GetToolkitFName() const { return FName("KishiRpyScriptEditor"); }
FText FRpyScriptEditor::GetBaseToolkitName() const { return FText::FromString(TEXT("Visual Novel Script")); }
FString FRpyScriptEditor::GetWorldCentricTabPrefix() const { return TEXT("Visual Novel Script"); }
FLinearColor FRpyScriptEditor::GetWorldCentricTabColorScale() const { return FLinearColor(0.12f, 0.45f, 0.7f, 0.5f); }

void FRpyScriptEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
	InTabManager->RegisterTabSpawner(InstructionTreeTabId, FOnSpawnTab::CreateSP(this, &FRpyScriptEditor::SpawnInstructionTreeTab))
		.SetDisplayName(FText::FromString(TEXT("Instructions")));
}

void FRpyScriptEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	InTabManager->UnregisterTabSpawner(InstructionTreeTabId);
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
}

TSharedRef<SDockTab> FRpyScriptEditor::SpawnInstructionTreeTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.Label(FText::FromString(TEXT("Instruction Tree")))
		[
			SNew(SRpyInstructionTree)
			.Script(ScriptAsset.Get())
		];
}
