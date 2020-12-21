#pragma once

#include "CoreMinimal.h"
#include "Templates/KishiInterfaceLinker.h"

#include "Libraries/Tree/BaseTree.h"

template <class ClassType,
          ClassType *(ClassType::*UGetParentTree)(),
          TArray<ClassType *> (ClassType::*UGetDirectChildren)(),
          uint8 (ClassType::*UGetDirectChildrenSize)() = NULL,
          class DerivedInterface = IBaseTree>
class TIBaseTreeClassLinker : public TKishiInterfaceLinker<DerivedInterface>
{
public:
    TIBaseTreeClassLinker(){};
    ~TIBaseTreeClassLinker(){};
    virtual TScriptInterface<IBaseTree> GetParentTree_Implementation() const override
    {
        return (((ClassType *)Link.Key)->*UGetParentTree)();
    };
    virtual TArray<TScriptInterface<IBaseTree>> GetDirectChildren_Implementation() const override
    {
        return (((ClassType *)Link.Key)->*UGetDirectChildren)();
    };

    virtual std::enable_if_t<UGetDirectChildrenSize != NULL, uint8> GetDirectChildrenSize_Implementation() const override
    {
        return (((ClassType *)Link.Key)->*UGetDirectChildrenSize)();
    };
    virtual std::enable_if_t<UGetDirectChildrenSize == NULL, uint8> GetDirectChildrenSize_Implementation() const override
    {
        return UBaseTreeLibrary::IGetDirectChildrenSize_Implementation(Link.Key);
    };
};