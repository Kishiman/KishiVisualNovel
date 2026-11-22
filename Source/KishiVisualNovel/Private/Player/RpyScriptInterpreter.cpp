
#include "Player/RpyScriptInterpreter.h"

#include "Player/RpyScriptInterpreterBaseManager.h"
#include "Script/RpyInstruction.h"
#include "Session/RpySession.h"

namespace
{
  /** The interpreter's managers that implement IRpyScriptInterpreterBaseManager, in audio, choice, scene, show, statement order. */
  TArray<UObject *, TInlineAllocator<5>> GetManagers(UObject *InterpreterObject)
  {
    TArray<UObject *, TInlineAllocator<5>> Managers;
    UObject *const Candidates[] = {
        IRpyScriptInterpreter::Execute_GetAudioManager(InterpreterObject).GetObject(),
        IRpyScriptInterpreter::Execute_GetChoiceManager(InterpreterObject).GetObject(),
        IRpyScriptInterpreter::Execute_GetSceneManager(InterpreterObject).GetObject(),
        IRpyScriptInterpreter::Execute_GetShowManager(InterpreterObject).GetObject(),
        IRpyScriptInterpreter::Execute_GetStatementManager(InterpreterObject).GetObject()};
    for (UObject *Manager : Candidates)
    {
      if (IsValid(Manager) && Manager->GetClass()->ImplementsInterface(URpyScriptInterpreterBaseManager::StaticClass()))
      {
        Managers.Add(Manager);
      }
    }
    return Managers;
  }
}

void URpyScriptInterpreterLibrary::HandleInstructionComplete_Default(const TScriptInterface<IRpyScriptInterpreter> &Target)
{
  UObject *InterpreterObject = Target.GetObject();
  if (!IsValid(InterpreterObject))
  {
    UE_LOG(LogTemp, Error, TEXT("HandleInstructionComplete_Default requires a valid interpreter."));
    return;
  }

  URpySession *Session = IRpyScriptInterpreter::Execute_GetSession(InterpreterObject);
  URpyInstruction *Instruction = IsValid(Session) ? Session->current : nullptr;
  if (!IsValid(Instruction))
  {
    UE_LOG(LogTemp, Error, TEXT("HandleInstructionComplete_Default requires a valid current instruction."));
    return;
  }
  for (UObject *Manager : GetManagers(InterpreterObject))
  {
    IRpyScriptInterpreterBaseManager::Execute_OnInstructionComplete(Manager);
  }

  if (!IsValid(Session) || Session->current != Instruction)
  {
    UE_LOG(LogTemp, Warning, TEXT("Ignoring instruction completion because a listener changed the current instruction."));
    return;
  }

  // Only an instruction that returned Wait is waiting for this completion; the session
  // already moved past anything that returned Continue or Failed.
  if (Instruction->LastResult != ERpyInstructionResult::Wait)
  {
    return;
  }

  Session->RunNext();
  if (!Session->current)
  {
    IRpyScriptInterpreter::Execute_Shutdown(InterpreterObject);
  }
}
void URpyScriptInterpreterLibrary::OnDialogueViseme_Default(const TScriptInterface<IRpyScriptInterpreter> &Target, FName name, EMouthViseme viseme, float timeOffset)
{
  UObject *InterpreterObject = Target.GetObject();
  if (!IsValid(InterpreterObject))
  {
    UE_LOG(LogTemp, Error, TEXT("OnDialogueViseme_Default requires a valid interpreter."));
    return;
  }
  UObject *const Listeners[] = {
      IRpyScriptInterpreter::Execute_GetAudioManager(InterpreterObject).GetObject(),
      IRpyScriptInterpreter::Execute_GetShowManager(InterpreterObject).GetObject()};
  for (UObject *Manager : Listeners)
  {
    if (IsValid(Manager) && Manager->GetClass()->ImplementsInterface(URpyScriptInterpreterBaseManager::StaticClass()))
    {
      IRpyScriptInterpreterBaseManager::Execute_OnDialogueViseme(Manager, name, viseme, timeOffset);
    }
  }
}
