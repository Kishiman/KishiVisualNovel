#pragma once

#include "RpyInstructionHelpers.h"

struct HideInstruction : public RpyInstruction
{
  FName tag;

  HideInstruction(URpyScript *script, FRpyLine *rpyLine, FName tag, FRpySceneOptions options) : RpyInstruction(script, rpyLine), tag(tag) { Props = PropsFromSceneOptions(options); };
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
