#include "AssignInstruction.h"
#include "Rpy/RpySession.h"

void UAssignInstruction::SetValue(URpySession *Session)
{
  if (!Session) return;
  if (valueType == TEXT("bool"))
    Session->runtimeData.bools.Add(varName, value == TEXT("true"));
  else if (valueType == TEXT("integer"))
    Session->runtimeData.ints.Add(varName, FCString::Atoi(*value));
  else if (valueType == TEXT("float"))
    Session->runtimeData.floats.Add(varName, FCString::Atof(*value));
  else
    Session->runtimeData.strings.Add(varName, value);
}
