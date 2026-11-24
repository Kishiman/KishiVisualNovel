#include "Player/RpyScriptInterpreterBaseManager.h"

#include "Templates/KishiScriptInterface.h"
#include "Session/RpySession.h"
#include "Script/RpyInstruction.h"

URpySession *URpyScriptInterpreterBaseManagerLibrary::GetSession(const TScriptInterface<IRpyScriptInterpreterBaseManager> &Target)
{
  TKishiScriptInterface<IRpyScriptInterpreterBaseManager> _Target = Target;
  TKishiScriptInterface<IRpyScriptInterpreter> interpreter = IRpyScriptInterpreterBaseManager::Execute_GetInterpreter(_Target.GetObject());
  if (!interpreter)
    return nullptr;
  return IRpyScriptInterpreter::Execute_GetSession(interpreter.GetObject());
}
EInstructionRunTimeType URpyScriptInterpreterBaseManagerLibrary::GetRunTimeType(const TScriptInterface<IRpyScriptInterpreterBaseManager> &Target)
{
  URpySession *session = URpyScriptInterpreterBaseManagerLibrary::GetSession(Target);
  if (!session)
    return EInstructionRunTimeType();
  return session->GetRunTimeType();
}
URpyInstruction *URpyScriptInterpreterBaseManagerLibrary::GetExecutingInstruction(const TScriptInterface<IRpyScriptInterpreterBaseManager> &Target)
{
  URpySession *Session = IsValid(Target.GetObject()) ? GetSession(Target) : nullptr;
  return Session ? Session->GetExecutingInstruction() : nullptr;
}

bool URpyScriptInterpreterBaseManagerLibrary::ReportInstructionComplete(const TScriptInterface<IRpyScriptInterpreterBaseManager> &Target, URpyInstruction *Instruction)
{
  UObject *Manager = Target.GetObject();
  if (!IsValid(Manager))
  {
    UE_LOG(LogTemp, Error, TEXT("ReportInstructionComplete requires a valid manager."));
    return false;
  }
  URpySession *Session = GetSession(Target);
  if (!IsValid(Session))
  {
    UE_LOG(LogTemp, Error, TEXT("%s: cannot report instruction completion without a session; the VN player sets the interpreter on its managers."), *Manager->GetName());
    return false;
  }
  return Session->CompleteInstruction(Instruction);
}

void URpyScriptInterpreterBaseManagerLibrary::RequestSkip(const TScriptInterface<IRpyScriptInterpreterBaseManager> &Target)
{
  UObject *Manager = Target.GetObject();
  if (!IsValid(Manager))
  {
    UE_LOG(LogTemp, Error, TEXT("RequestSkip requires a valid manager."));
    return;
  }
  UObject *InterpreterObject = IRpyScriptInterpreterBaseManager::Execute_GetInterpreter(Manager).GetObject();
  if (!IsValid(InterpreterObject) || !InterpreterObject->GetClass()->ImplementsInterface(URpyScriptInterpreter::StaticClass()))
  {
    UE_LOG(LogTemp, Error, TEXT("%s: cannot skip without an interpreter; the VN player sets it on its managers."), *Manager->GetName());
    return;
  }
  URpyScriptInterpreterLibrary::RequestSkip(TScriptInterface<IRpyScriptInterpreter>(InterpreterObject));
}
