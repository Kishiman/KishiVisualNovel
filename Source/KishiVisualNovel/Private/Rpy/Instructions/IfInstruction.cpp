#include "IfInstruction.h"
#include "ElseInstruction.h"

bool IfInstruction::Compile()
{
  RpyInstruction *current = this;
  while (current->next)
  {
    if (current->next->isOfType(RpyInstructionType::Else) && current->next->rpyLine.tabs == this->rpyLine.tabs)
    {
      current = current->next;
    }
    else
      break;
  }
  this->next = current->next;
  if (current != this)
  {
    this->elseInstruction = (ElseInstruction *)(current);
  }
  if (children.Num() > 0)
    children.Last()->next = this->next;
  return true;
}
RpyInstruction *IfInstruction::GetNext(URpySession *session)
{
  if (this->condition)
    return this->children[0];
  else if (this->elseInstruction)
    return this->elseInstruction;
  return this->next;
};
