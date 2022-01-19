#include "Interfaces/C++/RpyInstruction.h"


TScriptInterface<IRpyInstruction> URpyInstructionBase::GetNext(const TScriptInterface<IRpyInterpreter> &interpreter) {
  return this->next;
};
bool URpyInstructionBase::Execute(const TScriptInterface<IRpyInterpreter> &interpreter) {
  return false;
};

TScriptInterface<IBaseTree> URpyInstructionBase::GetParentTree_Implementation() const {
  return this->parent;
};

TArray<TScriptInterface<IBaseTree>> URpyInstructionBase::GetDirectChildren_Implementation() const {
    TArray<TScriptInterface<IBaseTree>> DirectChildren;
    DirectChildren.Reserve(this->children.Num());
    for(auto child : this->children){
      DirectChildren.Add(child);
    }
  return DirectChildren;
};

TScriptInterface<IArrayTree> URpyInstructionBase::GetChild_Implementation(uint8 index) const {
  return this->children[index];
};

void URpyInstructionBase::Compile(){
  // TScriptInterface<IArrayTree> me=this;
  // this->next=(URpyInstructionBase*)UArrayTreeLibrary::GetNext(me).GetObject();
}

