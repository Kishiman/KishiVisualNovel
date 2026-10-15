#include "ChoiceInstruction.h"
#include "MenuInstruction.h"

bool ChoiceInstruction::Compile()
{
  if (!parent->isOfType(RpyInstructionType::Menu))
  {
    UE_LOG(LogTemp, Error, TEXT("ChoiceInstruction not directly under MenuInstruction"));
    return false;
  }
  MenuInstruction *menu = (MenuInstruction *)(this->parent);
  if (children.Num() == 0)
  {
    UE_LOG(LogTemp, Error, TEXT("ChoiceInstruction has no children"));
    return false;
  }
  if (menu->choices.Num() == 0)
  {
    UE_LOG(LogTemp, Error, TEXT("MenuInstruction has no choices"));
    return false;
  }
  children.Last()->next = menu->choices.Last()->next;
  return true;
};
