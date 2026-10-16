#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "UObject/StrongObjectPtr.h"

class UEdGraph;
class URpyScript;
class IDetailsView;
class SGraphEditor;
class SVerticalBox;
class URpyInstruction;

/** Read-only control-flow graph for a compiled RpyScript. */
class SRpyInstructionTree : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SRpyInstructionTree) {}
		SLATE_ARGUMENT(URpyScript*, Script)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	void BuildGraph(URpyScript* Script);
	void OnGraphSelectionChanged(const TSet<UObject*>& NewSelection);
	void ShowSelectedBlock(class URpyInstructionGraphNode* Node);
	void InspectInstruction(URpyInstruction* Instruction);
	TWeakObjectPtr<URpyScript> Script;
	TStrongObjectPtr<UEdGraph> Graph;
	TSharedPtr<SGraphEditor> GraphEditor;
	TSharedPtr<IDetailsView> DetailsView;
	TSharedPtr<SVerticalBox> InstructionList;
};
