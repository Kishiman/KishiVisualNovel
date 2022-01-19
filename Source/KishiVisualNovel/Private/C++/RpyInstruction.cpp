#include "C++/RpyInstruction.h"

RpyInstructionBase::~RpyInstructionBase(){
  for(auto child : children){
    if(child)
      delete child;
  }
}

bool RpyInstructionBase::Compile(){
  return true;
}
bool RpyInstructionBase::Execute(const TScriptInterface<IRpyInterpreter> &interpreter) {
  return true;
};
RpyInstructionBase* RpyInstructionBase::GetNext(const TScriptInterface<IRpyInterpreter> &interpreter) {
  return this->next;
};
