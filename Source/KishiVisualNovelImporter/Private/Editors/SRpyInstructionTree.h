#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Views/STreeView.h"
#include "Rpy/RpyScript.h"

class RpyInstruction;
class IDetailsView;

struct FRpyInstructionTreeItem
{
	RpyInstruction* Instruction = nullptr;
	TArray<TSharedPtr<FRpyInstructionTreeItem>> Children;
};

class SRpyInstructionTree : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SRpyInstructionTree) {}
		SLATE_ARGUMENT(URpyScript*, Script)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	TSharedRef<ITableRow> GenerateRow(TSharedPtr<FRpyInstructionTreeItem> Item, const TSharedRef<STableViewBase>& OwnerTable) const;
	void GetItemChildren(TSharedPtr<FRpyInstructionTreeItem> Item, TArray<TSharedPtr<FRpyInstructionTreeItem>>& OutChildren) const;
	void OnSelectionChanged(TSharedPtr<FRpyInstructionTreeItem> Item, ESelectInfo::Type SelectionType);
	void RefreshInstructionDetails(RpyInstruction* Instruction);
	TSharedPtr<FRpyInstructionTreeItem> MakeTreeItem(RpyInstruction* Instruction) const;

	TWeakObjectPtr<URpyScript> Script;
	TArray<TSharedPtr<FRpyInstructionTreeItem>> RootItems;
	TSharedPtr<STreeView<TSharedPtr<FRpyInstructionTreeItem>>> TreeView;
	TSharedPtr<IDetailsView> DetailsView;
	TSharedPtr<SVerticalBox> SelectedInstructionDetails;
};
