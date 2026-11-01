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
    {
      UE_LOG(LogTemp, Error, TEXT("show: image '%s' is not defined in the script; skipping it."), *AssetKey.ToString());
      return true;
    }

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

    UObject *interpreter = session->interpreter.GetObject();
    UE_LOG(LogTemp, Log, TEXT("[VNShow] show instruction: name='%s' attribute='%s' resolved image tag='%s' type=%s interpreter=%s"),
           *name.ToString(), *options.attribute, *rpyImage->tag.ToString(), *UEnum::GetValueAsString(rpyImage->type), *GetNameSafe(interpreter));
    if (!IsValid(interpreter))
    {
      UE_LOG(LogTemp, Error, TEXT("[VNShow] show instruction: the session has no interpreter."));
      return true;
    }
    auto showManager = IRpyScriptInterpreter::Execute_GetShowManager(interpreter);
    UE_LOG(LogTemp, Log, TEXT("[VNShow] show instruction: show manager=%s class=%s"),
           *GetNameSafe(showManager.GetObject()), showManager.GetObject() ? *showManager.GetObject()->GetClass()->GetName() : TEXT("None"));
    if (!showManager.GetObject())
    {
      UE_LOG(LogTemp, Error, TEXT("[VNShow] show instruction: the interpreter %s returned no show manager."), *GetNameSafe(interpreter));
      return true;
    }

    const bool bResult = IRpyShowManager::Execute_Show(showManager.GetObject(), *rpyImage, options);
    UE_LOG(LogTemp, Log, TEXT("[VNShow] show instruction: Show returned %d"), bResult ? 1 : 0);
    return bResult;
  };
};
