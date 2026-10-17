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
  virtual bool Execute(URpySession *session)
  {
    const FRpySceneOptions options = SceneOptionsFromProps(Props, script);

    auto existing = ArrayLib::FindIndexByPredicate(session->showState.imageStates, [&](const FRpyShowImageState &state)
                                                   { return state.name == tag; });
    if (existing != INDEX_NONE)
    {
      session->showState.imageStates.RemoveAt(existing);
    }
    auto showManager = IRpyScriptInterpreter::Execute_GetShowManager(session->interpreter.GetObject());

    return IRpyShowManager::Execute_Hide(showManager.GetObject(), tag, options);
  };
};
