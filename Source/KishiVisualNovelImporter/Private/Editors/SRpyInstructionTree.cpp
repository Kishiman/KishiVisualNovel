#include "Editors/SRpyInstructionTree.h"

#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Rpy/RpyInstruction.h"
#include "Widgets/Layout/SSplitter.h"
#include "Widgets/Layout/SScrollBox.h"
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
				.OnSelectionChanged(this, &SRpyInstructionTree::OnSelectionChanged)
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
			SNew(SSplitter)
			.Orientation(Orient_Vertical)
			+ SSplitter::Slot()
			.Value(0.65f)
			[
				DetailsView.ToSharedRef()
			]
			+ SSplitter::Slot()
			.Value(0.35f)
			[
				SNew(SBorder)
				.Padding(6.0f)
				[
					SNew(SScrollBox)
					+ SScrollBox::Slot()
					[
						SAssignNew(SelectedInstructionDetails, SVerticalBox)
						+ SVerticalBox::Slot().AutoHeight().Padding(2.0f)
						[
							SNew(STextBlock).Text(FText::FromString(TEXT("Select an instruction to inspect its props.")))
						]
					]
				]
			]
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

void SRpyInstructionTree::OnSelectionChanged(TSharedPtr<FRpyInstructionTreeItem> Item, ESelectInfo::Type)
{
	RefreshInstructionDetails(Item.IsValid() ? Item->Instruction : nullptr);
}

void SRpyInstructionTree::RefreshInstructionDetails(RpyInstruction* Instruction)
{
	if (!SelectedInstructionDetails.IsValid()) return;
	SelectedInstructionDetails->ClearChildren();
	if (!Instruction)
	{
		SelectedInstructionDetails->AddSlot().AutoHeight().Padding(2.0f)
		[
			SNew(STextBlock).Text(FText::FromString(TEXT("Select an instruction to inspect its props.")))
		];
		return;
	}

	SelectedInstructionDetails->AddSlot().AutoHeight().Padding(2.0f)
	[
		SNew(STextBlock).Text(FText::FromString(Instruction->rpyLine.line))
	];
	if (Instruction->InstructionId.IsNone()) return;
	SelectedInstructionDetails->AddSlot().AutoHeight().Padding(2.0f)
	[
		SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("ID: %s"), *Instruction->InstructionId.ToString())))
	];
	if (Instruction->Props.Values.Num() == 0)
	{
		SelectedInstructionDetails->AddSlot().AutoHeight().Padding(2.0f)
		[
			SNew(STextBlock).Text(FText::FromString(TEXT("No optional props")))
		];
		return;
	}
	for (const FRpyInstructionProp& Prop : Instruction->Props.Values)
	{
		SelectedInstructionDetails->AddSlot().AutoHeight().Padding(2.0f)
		[
			SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("%s (%s): %s"), *Prop.Name.ToString(), *Prop.Type, *Prop.Value)))
		];
	}
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
