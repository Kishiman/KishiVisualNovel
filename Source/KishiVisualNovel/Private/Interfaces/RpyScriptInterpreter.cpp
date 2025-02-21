
#include "Interfaces/RpyScriptInterpreter.h"

#include "Interfaces/RpyScriptInterpreterBaseManager.h"
#include "Templates/KishiScriptInterface.h"

void URpyScriptInterpreterImplementation::IOnInstructionComplete_Default(const TScriptInterface<IRpyScriptInterpreter> &Target, EInstructionRunTimeType type)
{
  TKishiScriptInterface<IRpyScriptInterpreterBaseManager> audioManager = IRpyScriptInterpreter::Execute_GetAudioManager(Target.GetObject());
  if (audioManager)
  {
    IRpyScriptInterpreterBaseManager::Execute_OnInstructionComplete(audioManager.GetObject(), type);
  }
  TKishiScriptInterface<IRpyScriptInterpreterBaseManager> showManager = IRpyScriptInterpreter::Execute_GetShowManager(Target.GetObject());
  if (showManager)
  {
    IRpyScriptInterpreterBaseManager::Execute_OnInstructionComplete(showManager.GetObject(), type);
  }
}
void URpyScriptInterpreterImplementation::IOnDialogueViseme_Default(const TScriptInterface<IRpyScriptInterpreter> &Target, FName name, FMouthViseme viseme)
{
  TKishiScriptInterface<IRpyScriptInterpreterBaseManager> audioManager = IRpyScriptInterpreter::Execute_GetAudioManager(Target.GetObject());
  if (audioManager)
  {
    IRpyScriptInterpreterBaseManager::Execute_OnDialogueViseme(audioManager.GetObject(), name, viseme);
  }
  TKishiScriptInterface<IRpyScriptInterpreterBaseManager> showManager = IRpyScriptInterpreter::Execute_GetShowManager(Target.GetObject());
  if (showManager)
  {
    IRpyScriptInterpreterBaseManager::Execute_OnDialogueViseme(showManager.GetObject(), name, viseme);
  }
}