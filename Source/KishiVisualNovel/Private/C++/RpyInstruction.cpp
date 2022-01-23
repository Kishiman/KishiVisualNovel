#include "C++/RpyInstruction.h"

RpyInstruction::~RpyInstruction()
{
  for (auto child : children)
  {
    if (child)
      delete child;
  }
}

bool RpyInstruction::Compile()
{
  return true;
}
bool RpyInstruction::Execute(const TScriptInterface<IRpyInterpreter> &interpreter)
{
  UE_LOG(LogTemp, Display, TEXT("Executing rpy:%s"), (*this->rpyLine->line));
  return true;
};
RpyInstruction *RpyInstruction::GetNext(const TScriptInterface<IRpyInterpreter> &interpreter)
{
  return this->next;
};
