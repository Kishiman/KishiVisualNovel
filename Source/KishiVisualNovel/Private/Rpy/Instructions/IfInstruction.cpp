#include "IfInstruction.h"
#include "ElseInstruction.h"

bool UIfInstruction::Compile()
{
  URpyInstruction *current = this;
  while (current->next)
  {
    if (current->next->isOfType(RpyInstructionType::Else) && current->next->parent == this->parent)
    {
      current = current->next;
    }
    else
      break;
  }
  this->next = current->next;
  if (current != this)
  {
    this->elseInstruction = (UElseInstruction *)(current);
  }
  if (children.Num() > 0)
    children.Last()->next = this->next;
  return true;
}
URpyInstruction *UIfInstruction::GetNext(URpySession *session)
{
  if (this->condition)
    return this->children[0];
  else if (this->elseInstruction)
    return this->elseInstruction;
  return this->next;
};

TArray<URpyInstruction *> UIfInstruction::GetPossibleNextInstructions() const
{
  TArray<URpyInstruction *> Result;
  if (children.Num() > 0 && children[0])
    Result.Add(children[0]);
  URpyInstruction *FalseBranch = elseInstruction ? static_cast<URpyInstruction *>(elseInstruction) : next;
  if (FalseBranch)
    Result.AddUnique(FalseBranch);
  return Result;
}
