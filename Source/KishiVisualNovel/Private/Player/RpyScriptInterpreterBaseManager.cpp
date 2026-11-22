#include "Player/RpyScriptInterpreterBaseManager.h"

#include "Templates/KishiScriptInterface.h"
#include "Session/RpySession.h"

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
bool URpyScriptInterpreterBaseManagerLibrary::ReportInstructionComplete(const TScriptInterface<IRpyScriptInterpreterBaseManager> &Target)
{
  UObject *Manager = Target.GetObject();
  if (!IsValid(Manager))
  {
    UE_LOG(LogTemp, Error, TEXT("ReportInstructionComplete requires a valid manager."));
    return false;
  }
  UObject *InterpreterObject = IRpyScriptInterpreterBaseManager::Execute_GetInterpreter(Manager).GetObject();
  if (!IsValid(InterpreterObject) || !InterpreterObject->GetClass()->ImplementsInterface(URpyScriptInterpreter::StaticClass()))
  {
    UE_LOG(LogTemp, Error, TEXT("%s: cannot report instruction completion without an interpreter; the VN player sets it on its managers."), *Manager->GetName());
    return false;
  }
  IRpyScriptInterpreter::Execute_HandleInstructionComplete(InterpreterObject);
  return true;
}
