#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphSchema.h"
#include "Rpy/RpyInstruction.h"
#include "RpyInstructionGraphTypes.generated.h"

UCLASS()
class URpyInstructionGraphNode : public UEdGraphNode
{
  GENERATED_BODY()

public:
  UPROPERTY(Transient)
  TArray<URpyInstruction *> Instructions;
  UPROPERTY(Transient)
  TArray<URpyInstruction *> Successors;
  UPROPERTY(Transient)
  TArray<FString> SuccessorLabels;

  void ConfigureBlock(const TArray<URpyInstruction *> &InInstructions,
                      const TArray<URpyInstruction *> &InSuccessors,
                      const TArray<FString> &InSuccessorLabels);
  virtual void AllocateDefaultPins() override;
  virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
  UEdGraphPin *GetInputPin() const;
  UEdGraphPin *GetOutputPin(int32 Index) const;
};

UCLASS()
class URpyInstructionGraphSchema : public UEdGraphSchema
{
  GENERATED_BODY()

public:
  virtual class FConnectionDrawingPolicy *CreateConnectionDrawingPolicy(
      int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor,
      const class FSlateRect &InClippingRect, class FSlateWindowElementList &InDrawElements,
      UEdGraph *InGraphObj) const override;
};
