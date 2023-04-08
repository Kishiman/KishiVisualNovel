#include "C++/RpyInstructions.h"
#include "Interfaces/RpyInterpreter.h"

// Logic
bool LabelInstruction::Compile()
{
  this->next = nullptr;
  auto num = this->children.Num();
  if (num > 0)
  {
    this->children[num - 1]->next = nullptr;
  }
  return true;
}
bool SayInstruction::Execute(const TScriptInterface<IRpyInterpreter> &interpreter)
{
  RpyInstruction::Execute(interpreter);
  return IRpyInterpreter::Execute_Say(interpreter.GetObject(), this->name, this->statement);
};

bool SceneInstruction::Execute(const TScriptInterface<IRpyInterpreter> &interpreter)
{
  RpyInstruction::Execute(interpreter);
  return IRpyInterpreter::Execute_Scene(interpreter.GetObject(), this->params);
};

bool ShowInstruction::Execute(const TScriptInterface<IRpyInterpreter> &interpreter)
{
  RpyInstruction::Execute(interpreter);
  UPaperSprite *image = script->images[name];
  if (!image)
    return false;
  return IRpyInterpreter::Execute_Show(interpreter.GetObject(), image, at, with);
};
bool HideInstruction::Execute(const TScriptInterface<IRpyInterpreter> &interpreter)
{
  RpyInstruction::Execute(interpreter);
  return IRpyInterpreter::Execute_Hide(interpreter.GetObject(), this->name, this->params);
};
bool PlayInstruction::Execute(const TScriptInterface<IRpyInterpreter> &interpreter)
{
  RpyInstruction::Execute(interpreter);
  return IRpyInterpreter::Execute_PlayMusic(interpreter.GetObject(), this->name, this->params);
};

bool PauseInstruction::Execute(const TScriptInterface<IRpyInterpreter> &interpreter)
{
  RpyInstruction::Execute(interpreter);
  return IRpyInterpreter::Execute_Pause(interpreter.GetObject(), this->timeout);
};

RpyInstruction *IfInstruction::GetNext(const TScriptInterface<IRpyInterpreter> &interpreter)
{
  if (this->condition)
  {
    return this->children[0];
  }
  return this->next;
};

bool IfBoolInstruction::Execute(const TScriptInterface<IRpyInterpreter> &interpreter)
{
  this->condition = IRpyInterpreter::Execute_GetBool(interpreter.GetObject(), this->varName);
  this->condition = this->condition ^ this->reverse;
  return true;
};