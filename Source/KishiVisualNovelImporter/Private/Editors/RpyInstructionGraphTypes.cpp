#include "Editors/RpyInstructionGraphTypes.h"
#include "ConnectionDrawingPolicy.h"
#include "EdGraph/EdGraph.h"

void URpyInstructionGraphNode::ConfigureBlock(const TArray<URpyInstruction *> &InInstructions,
                                               const TArray<URpyInstruction *> &InSuccessors,
                                               const TArray<FString> &InSuccessorLabels)
{
  Instructions = InInstructions;
  Successors = InSuccessors;
  SuccessorLabels = InSuccessorLabels;
  if (SuccessorLabels.Num() < Successors.Num())
    SuccessorLabels.SetNum(Successors.Num());
}

void URpyInstructionGraphNode::AllocateDefaultPins()
{
  CreatePin(EGPD_Input, TEXT("exec"), TEXT("In"));
  for (int32 Index = 0; Index < Successors.Num(); ++Index)
  {
    const FString Label = SuccessorLabels.IsValidIndex(Index) && !SuccessorLabels[Index].IsEmpty()
                              ? SuccessorLabels[Index]
                              : FString::Printf(TEXT("next %d"), Index + 1);
    UEdGraphPin *Pin = CreatePin(EGPD_Output, TEXT("exec"), *Label);
    Pin->PinFriendlyName = FText::FromString(Label);
  }
}

FText URpyInstructionGraphNode::GetNodeTitle(ENodeTitleType::Type) const
{
  FString FirstInstruction = Instructions.Num() && Instructions[0] ? Instructions[0]->ToString() : TEXT("Empty block");
  if (FirstInstruction.Len() > 52)
    FirstInstruction = FirstInstruction.Left(49) + TEXT("...");
  if (Instructions.Num() > 1)
    FirstInstruction += FString::Printf(TEXT("  (+%d)"), Instructions.Num() - 1);
  return FText::FromString(FirstInstruction);
}

UEdGraphPin *URpyInstructionGraphNode::GetInputPin() const
{
  for (UEdGraphPin *Pin : Pins)
    if (Pin && Pin->Direction == EGPD_Input) return Pin;
  return nullptr;
}

UEdGraphPin *URpyInstructionGraphNode::GetOutputPin(int32 Index) const
{
  int32 OutputIndex = 0;
  for (UEdGraphPin *Pin : Pins)
  {
    if (Pin && Pin->Direction == EGPD_Output)
    {
      if (OutputIndex++ == Index) return Pin;
    }
  }
  return nullptr;
}

FConnectionDrawingPolicy *URpyInstructionGraphSchema::CreateConnectionDrawingPolicy(
    int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor,
    const FSlateRect &InClippingRect, FSlateWindowElementList &InDrawElements,
    UEdGraph *InGraphObj) const
{
  return new FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements);
}
