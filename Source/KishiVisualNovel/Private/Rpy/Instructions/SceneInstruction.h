#pragma once

#include "RpyInstructionHelpers.h"

struct SceneInstruction : public RpyInstruction
{
  FName name;

  SceneInstruction(URpyScript *script, FRpyLine *rpyLine, FName name, FRpySceneOptions options) : RpyInstruction(script, rpyLine), name(name) { Props = PropsFromSceneOptions(options); };
  virtual EInstructionRunTimeType RunTimeType() const { return EInstructionRunTimeType::SCENE; }
  virtual bool Compile() override
  {
    return name == FName(TEXT("_")) || ResolveImageForCompile(script, name, SceneOptionsFromProps(Props, script)) != nullptr;
  }
  virtual bool Execute(URpySession *session)
  {
    const FRpySceneOptions options = SceneOptionsFromProps(Props, script);
    FRpyImage rpyImage;
    if (name != "_")
    {
      FName AssetKey = name;
      if (!options.attribute.IsEmpty())
        AssetKey = FName(*(name.ToString() + TEXT(" ") + options.attribute));
      auto _rpyImage = script->images.Find(name);
      if (!_rpyImage && AssetKey != name)
        _rpyImage = script->images.Find(AssetKey);
      if (!_rpyImage)
        return false;
      rpyImage = *_rpyImage;
    }
    session->sceneState.rpyImage = rpyImage;
    session->sceneState.options = options;
    auto sceneManager = IRpyScriptInterpreter::Execute_GetSceneManager(session->interpreter.GetObject());
    return IRpySceneManager::Execute_Scene(sceneManager.GetObject(), rpyImage, options);
  };
};
