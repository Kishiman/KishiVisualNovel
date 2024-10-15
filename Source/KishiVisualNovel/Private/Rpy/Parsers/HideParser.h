#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"

// hide sylvie
struct HideParser : public RpyParser
{
  HideParser() : RpyParser(7, "^hide " + reg_multi_name + reg_rpy_scene_options + reg_comment, "HideParser") {};
  virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
  {

    FName tag = RpyParser::GetNames(*params[0])[0];
    auto options = GetRpySceneOptions(params, 1);
    for (auto &pair : script->images)
    {
      if (pair.Value.tag == tag)
        return new HideInstruction(script, rpyLine, tag, options);
    }
    for (auto &pair : script->layeredImages)
    {
      if (pair.Value.tag == tag)
        return new HideInstruction(script, rpyLine, tag, options);
    }
    return nullptr;
  };
};