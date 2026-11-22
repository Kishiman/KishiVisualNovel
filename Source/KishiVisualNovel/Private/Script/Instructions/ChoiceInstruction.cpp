#include "ChoiceInstruction.h"
#include "MenuInstruction.h"

bool UChoiceInstruction::Compile()
{
  if (!parent->isOfType(RpyInstructionType::Menu))
  {
    UE_LOG(LogTemp, Error, TEXT("UChoiceInstruction not directly under UMenuInstruction"));
    return false;
  }
  UMenuInstruction *menu = (UMenuInstruction *)(this->parent);
  if (children.Num() == 0)
  {
    UE_LOG(LogTemp, Error, TEXT("UChoiceInstruction has no children"));
    return false;
  }
  if (menu->choices.Num() == 0)
  {
    UE_LOG(LogTemp, Error, TEXT("UMenuInstruction has no choices"));
    return false;
  }
  children.Last()->next = menu->choices.Last()->next;
  return true;
};
