#include "PyInstruction.h"

TScriptInterface<IBaseTree>
PyInstruction::GetParentTree_Implementation() const {
  return this.parent;
};
TArray<TScriptInterface<IBaseTree>>
PyInstruction::GetDirectChildren_Implementation() const {
  return this.children;
};
TScriptInterface<IArrayTree>
PyInstruction::GetChild_Implementation(uint8 index) const {
  return this.children[index];
};
uint8 PyInstruction::GetDirectChildrenSize_Implementation() const {
  return this.children.Num();
};

TArray<FName> PyInstruction::GetAwaitEvents() const { return {}; };
