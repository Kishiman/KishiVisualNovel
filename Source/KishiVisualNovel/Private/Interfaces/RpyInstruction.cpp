#include "Interfaces/C++/RpyInstruction.h"

TScriptInterface<IBaseTree> IRpyInstruction::GetParentTree_Implementation() const
{
    return nullptr;
};

TArray<TScriptInterface<IBaseTree>> IRpyInstruction::GetDirectChildren_Implementation() const
{
    return {};
};

TScriptInterface<IArrayTree> IRpyInstruction::GetChild_Implementation(uint8 index) const
{
    return nullptr;
};