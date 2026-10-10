#include "AssetTypeActions_RpyScript.h"

#include "Editors/RpyScriptEditor.h"
#include "Rpy/RpyScript.h"

FText FAssetTypeActions_RpyScript::GetName() const { return FText::FromString(TEXT("Visual Novel Script")); }
FColor FAssetTypeActions_RpyScript::GetTypeColor() const { return FColor(50, 145, 205); }
UClass* FAssetTypeActions_RpyScript::GetSupportedClass() const { return URpyScript::StaticClass(); }
uint32 FAssetTypeActions_RpyScript::GetCategories() { return EAssetTypeCategories::Misc; }

void FAssetTypeActions_RpyScript::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (UObject* Object : InObjects)
	{
		if (URpyScript* Script = Cast<URpyScript>(Object))
		{
			TSharedRef<FRpyScriptEditor> Editor = MakeShared<FRpyScriptEditor>();
			Editor->InitRpyScriptEditor(Mode, EditWithinLevelEditor, Script);
		}
	}
}
