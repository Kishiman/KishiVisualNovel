#include "Interfaces/RpyScriptInterpreterBaseManager.h"

#include "Templates/KishiScriptInterface.h"
#include "Rpy/RpySession.h"

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