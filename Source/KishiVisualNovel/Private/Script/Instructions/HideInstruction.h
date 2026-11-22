#pragma once

#include "RpyInstructionHelpers.h"
#include "HideInstruction.generated.h"

UCLASS(BlueprintType)
class UHideInstruction : public URpyInstruction
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instruction")
  FName tag;
  virtual FString ToString() const override { return FormatInstruction(TEXT("hide"), FString::Printf(TEXT("tag=\"%s\""), *tag.ToString())); }
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::HIDE; }
  virtual ERpyInstructionResult Execute(URpySession *session)
  {
    const FRpySceneOptions options = SceneOptionsFromProps(Props, script);

    auto existing = ArrayLib::FindIndexByPredicate(session->showState.imageStates, [&](const FRpyShowImageState &state)
                                                   { return state.name == tag; });
    if (existing != INDEX_NONE)
    {
      session->showState.imageStates.RemoveAt(existing);
    }
    UObject *ShowManager = RpyFindManager(session, TEXT("hide"), TEXT("show manager"), [](UObject *Interpreter)
                                    { return IRpyScriptInterpreter::Execute_GetShowManager(Interpreter); });
    if (!ShowManager)
      return ERpyInstructionResult::Failed;

    return IRpyShowManager::Execute_Hide(ShowManager, tag, options);
  };
};
