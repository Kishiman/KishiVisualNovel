
#include "Interfaces/RpyScriptInterpreter.h"

#include "Interfaces/RpyScriptInterpreterBaseManager.h"
#include "Templates/KishiScriptInterface.h"
#include "Rpy/RpyInstruction.h"
#include "Rpy/RpySession.h"

void URpyScriptInterpreterImplementation::IHandleInstructionComplete_Default(const TScriptInterface<IRpyScriptInterpreter> &Target)
{
  UObject *InterpreterObject = Target.GetObject();
  if (!IsValid(InterpreterObject))
  {
    UE_LOG(LogTemp, Error, TEXT("IHandleInstructionComplete_Default requires a valid interpreter."));
    return;
  }

  URpySession *Session = IRpyScriptInterpreter::Execute_GetSession(InterpreterObject);
  URpyInstruction *Instruction = IsValid(Session) ? Session->current : nullptr;
  if (!IsValid(Instruction))
  {
    UE_LOG(LogTemp, Error, TEXT("IHandleInstructionComplete_Default requires a valid current instruction."));
    return;
  }
  TKishiScriptInterface<IRpyScriptInterpreterBaseManager> audioManager = IRpyScriptInterpreter::Execute_GetAudioManager(Target.GetObject());
  if (audioManager)
  {
    IRpyScriptInterpreterBaseManager::Execute_OnInstructionComplete(audioManager.GetObject());
  }
  TKishiScriptInterface<IRpyScriptInterpreterBaseManager> choiceManager = IRpyScriptInterpreter::Execute_GetChoiceManager(Target.GetObject());
  if (choiceManager)
  {
    IRpyScriptInterpreterBaseManager::Execute_OnInstructionComplete(choiceManager.GetObject());
  }
  TKishiScriptInterface<IRpyScriptInterpreterBaseManager> sceneManager = IRpyScriptInterpreter::Execute_GetSceneManager(Target.GetObject());
  if (sceneManager)
  {
    IRpyScriptInterpreterBaseManager::Execute_OnInstructionComplete(sceneManager.GetObject());
  }
  TKishiScriptInterface<IRpyScriptInterpreterBaseManager> showManager = IRpyScriptInterpreter::Execute_GetShowManager(Target.GetObject());
  if (showManager)
  {
    IRpyScriptInterpreterBaseManager::Execute_OnInstructionComplete(showManager.GetObject());
  }
  TKishiScriptInterface<IRpyScriptInterpreterBaseManager> statementManager = IRpyScriptInterpreter::Execute_GetStatementManager(Target.GetObject());
  if (statementManager)
  {
    IRpyScriptInterpreterBaseManager::Execute_OnInstructionComplete(statementManager.GetObject());
  }

  if (!IsValid(Session) || Session->current != Instruction)
  {
    UE_LOG(LogTemp, Warning, TEXT("Ignoring instruction completion because a listener changed the current instruction."));
    return;
  }

  if (Instruction->bAutoExecuteNext)
  {
    return;
  }

  Session->RunNext();
  if (!Session->current)
  {
    IRpyScriptInterpreter::Execute_Shutdown(InterpreterObject);
  }
}
void URpyScriptInterpreterImplementation::IOnDialogueViseme_Default(const TScriptInterface<IRpyScriptInterpreter> &Target, FName name, EMouthViseme viseme, float timeOffset)
{
  TKishiScriptInterface<IRpyScriptInterpreterBaseManager> audioManager = IRpyScriptInterpreter::Execute_GetAudioManager(Target.GetObject());
  if (audioManager)
  {
    IRpyScriptInterpreterBaseManager::Execute_OnDialogueViseme(audioManager.GetObject(), name, viseme, timeOffset);
  }
  TKishiScriptInterface<IRpyScriptInterpreterBaseManager> showManager = IRpyScriptInterpreter::Execute_GetShowManager(Target.GetObject());
  if (showManager)
  {
    IRpyScriptInterpreterBaseManager::Execute_OnDialogueViseme(showManager.GetObject(), name, viseme, timeOffset);
  }
}
