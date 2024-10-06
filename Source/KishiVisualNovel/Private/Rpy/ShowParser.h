
#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Rpy/RpyParser.h"
#include "Rpy/RpyInstructions.h"

// ^show
// show john happy
// show sylvie green smile with dissolve
struct ShowParser : public RpyParser
{
  ShowParser() : RpyParser(7, "^show " + reg_multi_name + reg_rpy_scene_options + reg_comment, "ShowParser") {};
  virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
  {
    FName name = FName(*params[0]);
    auto options = GetRpySceneOptions(params, 1);
    auto names = RpyParser::GetNames(name.ToString());
    auto mainName = names[0];
    auto image = script->images.Find(name);
    auto layeredImage = script->layeredImages.Find(mainName);
    if (!(image || layeredImage))
    {
      if (!script->AddDefaultImage(name, params[0]))
        return nullptr;
    }
    image = script->images.Find(name);
    if (image)
      return new ShowInstruction(script, rpyLine, name, options);
    layeredImage = script->layeredImages.Find(mainName);
    if (layeredImage)
    {
      names.RemoveAt(0);
      return new ShowLayeredInstruction(script, rpyLine, mainName, names, options);
    }
    return nullptr;
  };
};