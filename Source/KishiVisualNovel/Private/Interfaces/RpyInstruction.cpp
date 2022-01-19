#include "Interfaces/C++/RpyInstruction.h"

RpyInstructionBase::~RpyInstructionBase(){
  for(auto child : children){
    if(child)
      delete child;
  }
}

bool RpyInstructionBase::Compile(){
  return true;
  // TScriptInterface<IArrayTree> me=this;
  // this->next=(RpyInstructionBase*)UArrayTreeLibrary::GetNext(me).GetObject();
}
bool RpyInstructionBase::Execute(const TScriptInterface<IRpyInterpreter> &interpreter) {
  return true;
};
RpyInstructionBase* RpyInstructionBase::GetNext(const TScriptInterface<IRpyInterpreter> &interpreter) {
  return this->next;
};

TScriptInterface<IBaseTree> RpyInstructionBase::GetParentTree_Implementation() const {
  return this->parent;
};

TArray<TScriptInterface<IBaseTree>> RpyInstructionBase::GetDirectChildren_Implementation() const {
    TArray<TScriptInterface<IBaseTree>> DirectChildren;
    DirectChildren.Reserve(this->children.Num());
    for(auto child : this->children){
      DirectChildren.Add(child);
    }
  return DirectChildren;
};

TScriptInterface<IArrayTree> RpyInstructionBase::GetChild_Implementation(uint8 index) const {
  return this->children[index];
};


