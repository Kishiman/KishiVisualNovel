#include "Editors/SRpyInstructionTree.h"

#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "Editors/RpyInstructionGraphTypes.h"
#include "GraphEditor.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Rpy/RpyInstruction.h"
#include "Rpy/RpyScript.h"
#include "Widgets/Layout/SSplitter.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"

void SRpyInstructionTree::Construct(const FArguments& InArgs)
{
	Script = InArgs._Script;
	FDetailsViewArgs DetailsArgs;
	DetailsArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsArgs.bUpdatesFromSelection = false;
	DetailsArgs.bLockable = false;
	DetailsArgs.bAllowSearch = true;
	FPropertyEditorModule& PropertyEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	DetailsView = PropertyEditor.CreateDetailView(DetailsArgs);
	DetailsView->SetObject(Script.Get());

	BuildGraph(Script.Get());
	ChildSlot
	[
		SNew(SSplitter)
		+ SSplitter::Slot().Value(0.72f)
		[
			GraphEditor.ToSharedRef()
		]
		+ SSplitter::Slot().Value(0.28f)
		[
			SNew(SSplitter)
			.Orientation(Orient_Vertical)
			+ SSplitter::Slot().Value(0.4f)
			[
				SNew(SBorder).Padding(6.f)
				[
					SNew(SScrollBox)
					+ SScrollBox::Slot()
					[
						SAssignNew(InstructionList, SVerticalBox)
						+ SVerticalBox::Slot().AutoHeight().Padding(2.f)
						[
							SNew(STextBlock).Text(FText::FromString(TEXT("Select a graph block to list its instructions.")))
						]
					]
				]
			]
			+ SSplitter::Slot().Value(0.6f)
			[
				DetailsView.ToSharedRef()
			]
		]
	];
}

void SRpyInstructionTree::BuildGraph(URpyScript* ScriptAsset)
{
	Graph = TStrongObjectPtr<UEdGraph>(NewObject<UEdGraph>(GetTransientPackage(), NAME_None, RF_Transient));
	Graph->Schema = URpyInstructionGraphSchema::StaticClass();
	if (ScriptAsset)
	{
		TMap<URpyInstruction*, int32> BlockForInstruction;
		TArray<URpyInstructionGraphNode*> Nodes;
		TArray<URpyInstruction*> CurrentBlock;
		int32 Row = 0;
		auto FlushBlock = [&]()
		{
			if (CurrentBlock.Num() == 0) return;
			URpyInstructionGraphNode* Node = NewObject<URpyInstructionGraphNode>(Graph.Get(), NAME_None, RF_Transient);
			Graph->AddNode(Node, false, false);
			Node->NodePosX = 0;
			Node->NodePosY = Row++ * 180;
			for (URpyInstruction* Instruction : CurrentBlock) BlockForInstruction.Add(Instruction, Nodes.Num());
			Node->ConfigureBlock(CurrentBlock, TArray<URpyInstruction*>(), TArray<FString>());
			Nodes.Add(Node);
			CurrentBlock.Reset();
		};

		// Source order lets ordinary dialogue and presentation commands share a block.
		for (URpyInstruction* Instruction : ScriptAsset->instructions)
		{
			if (!Instruction) continue;
			if (CurrentBlock.Num() > 0)
			{
				URpyInstruction* Previous = CurrentBlock.Last();
				const TArray<URpyInstruction*> PreviousNext = Previous->GetPossibleNextInstructions();
				if (Previous->IsControlFlowBoundary() || PreviousNext.Num() != 1 || PreviousNext[0] != Instruction)
					FlushBlock();
			}
			CurrentBlock.Add(Instruction);
			if (Instruction->IsControlFlowBoundary()) FlushBlock();
		}
		FlushBlock();

		// Ensure branch targets outside the source-order list are still visible.
		for (URpyInstruction* Instruction : ScriptAsset->instructions)
		{
			if (!Instruction || BlockForInstruction.Contains(Instruction)) continue;
			CurrentBlock.Add(Instruction);
			FlushBlock();
		}

		// Assign block successor arrays after all instructions have a node mapping.
		for (int32 NodeIndex = 0; NodeIndex < Nodes.Num(); ++NodeIndex)
		{
			URpyInstructionGraphNode* Node = Nodes[NodeIndex];
			URpyInstruction* Last = Node->Instructions.Num() ? Node->Instructions.Last() : nullptr;
			if (!Last) continue;
			const TArray<URpyInstruction*> Successors = Last->GetPossibleNextInstructions();
			const TArray<FString> Labels = Last->GetPossibleNextInstructionLabels();
			TArray<URpyInstruction*> TargetInstructions;
			TArray<FString> TargetLabels;
			for (int32 Index = 0; Index < Successors.Num(); ++Index)
			{
				URpyInstruction* Successor = Successors[Index];
				if (!Successor || !BlockForInstruction.Contains(Successor)) continue;
				const int32 TargetNodeIndex = BlockForInstruction[Successor];
				if (TargetNodeIndex == NodeIndex) continue;
				TargetInstructions.Add(Successor);
				TargetLabels.Add(Labels.IsValidIndex(Index) ? Labels[Index] : TEXT("next"));
			}
			Node->ConfigureBlock(Node->Instructions, TargetInstructions, TargetLabels);
			Node->CreateNewGuid();
		}

		// Create every socket before wiring edges; destination nodes may appear later in source order.
		for (URpyInstructionGraphNode* Node : Nodes)
			Node->AllocateDefaultPins();

		for (int32 NodeIndex = 0; NodeIndex < Nodes.Num(); ++NodeIndex)
		{
			URpyInstructionGraphNode* Node = Nodes[NodeIndex];
			for (int32 Index = 0; Index < Node->Successors.Num(); ++Index)
			{
				const int32 TargetNodeIndex = BlockForInstruction[Node->Successors[Index]];
				if (!Nodes.IsValidIndex(TargetNodeIndex)) continue;
				UEdGraphPin* Output = Node->GetOutputPin(Index);
				UEdGraphPin* Input = Nodes[TargetNodeIndex]->GetInputPin();
				if (Output && Input)
				{
					Output->MakeLinkTo(Input);
				}
			}
		}

		// Degree is the shortest number of edges from the selected graph root.
		TMap<URpyInstructionGraphNode*, int32> Degrees;
		TArray<URpyInstructionGraphNode*> Pending;
		URpyInstructionGraphNode* RootNode = Nodes.Num() ? Nodes[0] : nullptr;
		if (ScriptAsset->init)
			if (const int32* RootIndex = BlockForInstruction.Find(ScriptAsset->init))
				if (Nodes.IsValidIndex(*RootIndex)) RootNode = Nodes[*RootIndex];
		if (RootNode)
		{
			Degrees.Add(RootNode, 0);
			Pending.Add(RootNode);
		}
		for (int32 QueueIndex = 0; QueueIndex < Pending.Num(); ++QueueIndex)
		{
			URpyInstructionGraphNode* Node = Pending[QueueIndex];
			const int32 NextDegree = Degrees[Node] + 1;
			for (URpyInstruction* Successor : Node->Successors)
			{
				const int32* TargetIndex = BlockForInstruction.Find(Successor);
				if (!TargetIndex || !Nodes.IsValidIndex(*TargetIndex)) continue;
				URpyInstructionGraphNode* Target = Nodes[*TargetIndex];
				int32* ExistingDegree = Degrees.Find(Target);
				if (!ExistingDegree || NextDegree < *ExistingDegree)
				{
					Degrees.Add(Target, NextDegree);
					Pending.Add(Target);
				}
			}
		}

		int32 UnreachableDegree = 1;
		for (const TPair<URpyInstructionGraphNode*, int32>& Entry : Degrees)
			UnreachableDegree = FMath::Max(UnreachableDegree, Entry.Value + 1);
		TMap<int32, int32> VerticalOffsetAtDegree;
		for (URpyInstructionGraphNode* Node : Nodes)
		{
			const int32* FoundDegree = Degrees.Find(Node);
			const int32 Degree = FoundDegree ? *FoundDegree : UnreachableDegree;
			int32& VerticalOffset = VerticalOffsetAtDegree.FindOrAdd(Degree);
			Node->NodePosX = Degree * 600;
			Node->NodePosY = VerticalOffset;
			const int32 EstimatedHeight = 140 + Node->Successors.Num() * 28;
			VerticalOffset += FMath::Max(280, EstimatedHeight + 80);
		}
		Graph->NotifyGraphChanged();
	}

	SGraphEditor::FGraphEditorEvents Events;
	Events.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &SRpyInstructionTree::OnGraphSelectionChanged);
	GraphEditor = SNew(SGraphEditor).GraphToEdit(Graph.Get()).GraphEvents(Events).IsEditable(false);
}

void SRpyInstructionTree::OnGraphSelectionChanged(const TSet<UObject*>& NewSelection)
{
	for (UObject* Object : NewSelection)
	{
		if (URpyInstructionGraphNode* Node = Cast<URpyInstructionGraphNode>(Object))
		{
			ShowSelectedBlock(Node);
			return;
		}
	}
	ShowSelectedBlock(nullptr);
	DetailsView->SetObject(Script.Get());
}

void SRpyInstructionTree::ShowSelectedBlock(URpyInstructionGraphNode* Node)
{
	if (!InstructionList.IsValid()) return;
	InstructionList->ClearChildren();
	InstructionList->AddSlot().AutoHeight().Padding(2.f)
	[
		SNew(STextBlock).Text(FText::FromString(Node ? FString::Printf(TEXT("Instructions in block (%d)"), Node->Instructions.Num()) : TEXT("No block selected")))
	];
	if (!Node || Node->Instructions.Num() == 0)
	{
		DetailsView->SetObject(Script.Get());
		return;
	}
	for (int32 Index = 0; Index < Node->Instructions.Num(); ++Index)
	{
		URpyInstruction* Instruction = Node->Instructions[Index];
		if (!Instruction) continue;
		InstructionList->AddSlot().AutoHeight().Padding(2.f)
		[
			SNew(SButton)
			.OnClicked_Lambda([this, Instruction]() { InspectInstruction(Instruction); return FReply::Handled(); })
			[
				SNew(STextBlock)
				.Text(FText::FromString(FString::Printf(TEXT("%d. %s"), Index + 1, *Instruction->ToString())))
				.AutoWrapText(true)
			]
		];
	}
	InspectInstruction(Node->Instructions[0]);
}

void SRpyInstructionTree::InspectInstruction(URpyInstruction* Instruction)
{
	DetailsView->SetObject(Instruction);
}
