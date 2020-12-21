#pragma once

#include "CoreMinimal.h"
#include "Templates/KishiInterfaceLinker.h"
#include "Libraries/Tree/ArrayTree.h"

template <class ClassType,
          ClassType *(ClassType::*UGetParentTree)(),
          TArray<ClassType *> (ClassType::*UGetDirectChildren)(),
          uint8 (ClassType::*UGetDirectChildrenSize)() = NULL,
          ClassType *(ClassType::*UGetChild)(uint8) = NULL,
          class DerivedInterface = IArrayTree>
class TIArrayTree : public TKishiInterfaceLinker<DerivedInterface>
{
public:
    TIArrayTree(){};
    ~TIArrayTree(){};

    virtual std::enable_if_t<UGetChild != NULL, TScriptInterface<IArrayTree>> GetChild_Implementation(uint8 index) const
    {
        return (((ClassType *)Link.Key)->*UGetChild)(index);
    };
    virtual std::enable_if_t<UGetChild == NULL, TScriptInterface<IArrayTree>> GetChild_Implementation(uint8 index) const
    {
        return UArrayTreeLibrary::IGetChild_Implementation(Link.Key, index);
    };
};