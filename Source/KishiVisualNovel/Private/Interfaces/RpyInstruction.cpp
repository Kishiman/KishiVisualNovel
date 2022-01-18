#include "Interfaces/C++/RpyInstruction.h"

bool IRpyInstruction::IsAsync() const { return this->isAsync; };

TScriptInterface<IRpyInterpreter> IRpyInstruction::GetNext(const TScriptInterface<IRpyInterpreter> &interpreter) {
  return this->next;
};

TScriptInterface<IBaseTree> IRpyInstruction::GetParentTree_Implementation() const {
  return this->parent;
};

TArray<TScriptInterface<IBaseTree>> IRpyInstruction::GetDirectChildren_Implementation() const {
  return this->children;
};

TScriptInterface<IArrayTree> IRpyInstruction::GetChild_Implementation(uint8 index) const {
  return this->children[index];
};

void IRpyInstruction::Compile(){
    this->next=UArrayTreeLibrary::GetNext(this);
}

