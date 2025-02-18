
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
  ShowParser() : RpyParser(7, "^show " + reg_multi_name + reg_rpy_scene_options_6 + reg_comment_nc, "ShowParser") {};
  virtual RpyInstruction *GetRpyInstruction(URpyScript *script, FRpyLine *rpyLine, TArray<FString> params)
  {
    // FString mainString;
    // FString attribute;
    // params[0].Split(" ", &mainString, &attribute);
    FString attribute = params[0];
    FName name = FName(*params[0]);
    auto options = GetRpySceneOptions(script, params, 1);
    options.attribute = attribute;
    auto names = RpyParser::GetNames(name.ToString());
    auto mainName = names[0];
    auto image = script->images.Find(name);
    if (!(image))
    {
      if (!script->AddDefaultImage(name, params[0]))
        return nullptr;
    }
    image = script->images.Find(name);
    if (image)
      return new ShowInstruction(script, rpyLine, name, options);
    image = script->images.Find(mainName);
    if (image)
      return new ShowInstruction(script, rpyLine, mainName, options);
    return nullptr;
  };
};