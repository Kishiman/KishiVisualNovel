
#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"

struct SceneParser : public RpyParser
{
  SceneParser() : RpyParser(7, "^scene " + reg_multi_name + reg_rpy_scene_options + reg_comment, "SceneParser")
  {
    UE_LOG(LogTemp, Warning, TEXT("reg_rpy_scene_options:%s"), *FString(RpyParser::reg_rpy_scene_options.c_str()));
  };
  virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
  {
    FName name = FName(*params[0]);
    auto names = RpyParser::GetNames(name.ToString());
    if (!script->images.Contains(name))
      if (!script->AddDefaultImage(name, params[0]))
        return nullptr;

    auto options = GetRpySceneOptions(params, 1);
    return new SceneInstruction(script, rpyLine, name, options);
  };
};