#pragma once

#include "RpyInstructionHelpers.h"
#include "ShowInstruction.generated.h"

UCLASS(BlueprintType)
class UShowInstruction : public URpyInstruction
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instruction")
  FName name;
  virtual FString ToString() const override { return FormatInstruction(TEXT("show"), FString::Printf(TEXT("asset=\"%s\""), *name.ToString())); }
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::SHOW; }
  virtual bool Compile() override
  {
    return ResolveImageForCompile(script, name, SceneOptionsFromProps(Props, script)) != nullptr;
  }
  virtual bool Execute(URpySession *session)
  {
    const FRpySceneOptions options = SceneOptionsFromProps(Props, script);
    FName AssetKey = name;
    if (!options.attribute.IsEmpty())
      AssetKey = FName(*(name.ToString() + TEXT(" ") + options.attribute));
    auto rpyImage = script->images.Find(name);
    if (!rpyImage && AssetKey != name)
      rpyImage = script->images.Find(AssetKey);
    if (!rpyImage)
      return false;

    auto existing = session->showState.imageStates.FindByPredicate([&](const FRpyShowImageState &state)
                                                                   { return state.name == name; });
    if (existing)
    {
      existing->rpyImage = *rpyImage;
      existing->options = options;
    }
    else
    {
      session->showState.imageStates.Add(FRpyShowImageState{name, *rpyImage, options});
    }

    auto showManager = IRpyScriptInterpreter::Execute_GetShowManager(session->interpreter.GetObject());

    return IRpyShowManager::Execute_Show(showManager.GetObject(), *rpyImage, options);
  };
};
