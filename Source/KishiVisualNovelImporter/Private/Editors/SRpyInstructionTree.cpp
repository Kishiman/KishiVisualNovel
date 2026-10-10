#include "Editors/SRpyInstructionTree.h"

#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Rpy/RpyInstruction.h"
#include "Widgets/Layout/SSplitter.h"
#include "Widgets/Views/STreeView.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Text/STextBlock.h"

void SRpyInstructionTree::Construct(const FArguments& InArgs)
{
	Script = InArgs._Script;
	if (URpyScript* ScriptAsset = Script.Get())
	{
		FDetailsViewArgs DetailsArgs;
		DetailsArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
		DetailsArgs.bUpdatesFromSelection = false;
		DetailsArgs.bLockable = false;
		DetailsArgs.bAllowSearch = true;
		DetailsArgs.bHideSelectionTip = true;
		FPropertyEditorModule& PropertyEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
		DetailsView = PropertyEditor.CreateDetailView(DetailsArgs);
		DetailsView->SetObject(ScriptAsset);

		for (RpyInstruction* Instruction : ScriptAsset->instructions)
		{
			if (Instruction && !Instruction->parent)
			{
				RootItems.Add(MakeTreeItem(Instruction));
			}
		}
	}

	ChildSlot
	[
		SNew(SSplitter)
		+ SSplitter::Slot()
		.Value(0.6f)
		[
			SNew(SBorder)
			.Padding(4.0f)
			[
				SAssignNew(TreeView, STreeView<TSharedPtr<FRpyInstructionTreeItem>>)
				.TreeItemsSource(&RootItems)
				.OnGenerateRow(this, &SRpyInstructionTree::GenerateRow)
				.OnGetChildren(this, &SRpyInstructionTree::GetItemChildren)
				.SelectionMode(ESelectionMode::Single)
				.HeaderRow
				(
					SNew(SHeaderRow)
					+ SHeaderRow::Column("Instruction")
					.DefaultLabel(FText::FromString(TEXT("Instruction")))
				)
			]
		]
		+ SSplitter::Slot()
		.Value(0.4f)
		[
			DetailsView.ToSharedRef()
		]
	];
}

TSharedRef<ITableRow> SRpyInstructionTree::GenerateRow(TSharedPtr<FRpyInstructionTreeItem> Item, const TSharedRef<STableViewBase>& OwnerTable) const
{
	FString Label = Item.IsValid() && Item->Instruction ? Item->Instruction->rpyLine.line : TEXT("Invalid instruction");
	return SNew(STableRow<TSharedPtr<FRpyInstructionTreeItem>>, OwnerTable)
	[
		SNew(STextBlock)
		.Text(FText::FromString(Label))
	];
}

void SRpyInstructionTree::GetItemChildren(TSharedPtr<FRpyInstructionTreeItem> Item, TArray<TSharedPtr<FRpyInstructionTreeItem>>& OutChildren) const
{
	if (Item.IsValid()) OutChildren = Item->Children;
}

TSharedPtr<FRpyInstructionTreeItem> SRpyInstructionTree::MakeTreeItem(RpyInstruction* Instruction) const
{
	TSharedPtr<FRpyInstructionTreeItem> Item = MakeShared<FRpyInstructionTreeItem>();
	Item->Instruction = Instruction;
	if (Instruction)
	{
		for (RpyInstruction* Child : Instruction->children)
		{
			if (Child) Item->Children.Add(MakeTreeItem(Child));
		}
	}
	return Item;
}
