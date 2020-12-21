#include "Libraries/Tree/BaseTree.h"
#include "Templates/KishiScriptInterface.h"
#include "Links/Tree/BaseTree.h"
#include "Macros/Interface.h"

TKishiScriptInterface<IBaseTree> UBaseTreeLibrary::IGetParentTree(TKishiScriptInterface<IBaseTree> Target)
{
    checkf(Target.GetObject() != NULL, TEXT("Error at UBaseTreeLibrary::IGetParentTree : Target is NULL or does not implement Interface IBaseTree\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    // checkCode(
        do{
        auto parent = IBaseTree::Execute_GetParentTree(Target.GetObject());
        while (parent.GetObject()) {
            checkf(Target.GetObject() != parent.GetObject(), TEXT("Error at UBaseTreeLibrary::IGetParentTree : Tree Has Cycle, target is parent of itself"));
            parent = IBaseTree::Execute_GetParentTree(parent.GetObject());
        }
        // );
        }while(false);
    // checkCode(
        do{
        auto parent = IBaseTree::Execute_GetParentTree(Target.GetObject());
        if (parent.GetObject()) {
            auto children = IBaseTree::Execute_GetDirectChildren(parent.GetObject());
            checkf(children.Contains(Target), TEXT("Error at UBaseTreeLibrary::IGetParentTree : Broken Tree, KishiTarget not in parent children"))
        }
        // );
        }while(false);
    auto parent = IBaseTree::Execute_GetParentTree(Target.GetObject());
    return parent;
}

TArray<TScriptInterface<IBaseTree>> UBaseTreeLibrary::IGetDirectChildren(TKishiScriptInterface<IBaseTree> Target)
{
    checkf(Target.GetObject() != NULL, TEXT("Error at UBaseTreeLibrary::IGetDirectChildren : Target is NULL or does not implement Interface IBaseTree\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    // checkCode(
        do{
        auto children = IBaseTree::Execute_GetDirectChildren(Target.GetObject());
        for (auto &&child
             : children) {
            checkf(IGetParentTree(child) == Target, TEXT("Error at UBaseTreeLibrary::IGetParentTree : Broken Tree, child not connected to target"));
        }
        // );
        }while(false);
    auto children = IBaseTree::Execute_GetDirectChildren(Target.GetObject());
    return children;
}

uint8 UBaseTreeLibrary::IGetDirectChildrenSize(TKishiScriptInterface<IBaseTree> Target)
{
    checkf(Target.GetObject() != NULL, TEXT("Error at UBaseTreeLibrary::IGetDirectChildrenSize : Target is NULL or does not implement Interface IBaseTree\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    uint8 childrenSize = IBaseTree::Execute_GetDirectChildrenSize(Target.GetObject());
    // checkCode(
        do{
        auto children = IGetDirectChildren(Target);
        checkf(childrenSize == children.Num(), TEXT("Error at UBaseTreeLibrary::IGetParentTree : Broken Tree, children size don't match children returned"));
        // );
        }while(false);
    return childrenSize;
}

uint8 UBaseTreeLibrary::IGetDirectChildrenSize_Default(const TScriptInterface<IBaseTree> &Target)
{
    auto children = IGetDirectChildren(Target);
    return children.Num();
}

bool UBaseTreeLibrary::IsLeaf(const TScriptInterface<IBaseTree> &Target)
{
    auto children = IGetDirectChildren(Target);
    return children.Num() == 0;
}

bool UBaseTreeLibrary::IsRoot(const TScriptInterface<IBaseTree> &Target)
{
    auto parent = IGetParentTree(Target);
    return parent.GetObject()==NULL;
}

bool UBaseTreeLibrary::IsDirectParentOf(const TScriptInterface<IBaseTree> &Target, const TScriptInterface<IBaseTree> &Child)
{
    auto parent = IGetParentTree(Child);
    return (Target.GetObject()!=NULL) && parent == Target;
}

bool UBaseTreeLibrary::IsDirectChildOf(const TScriptInterface<IBaseTree> &Target, const TScriptInterface<IBaseTree> &Parent)
{
    return IsDirectParentOf(Parent, Target);
}

bool UBaseTreeLibrary::IsParentOf(const TScriptInterface<IBaseTree> &Target, const TScriptInterface<IBaseTree> &Child, TArray<TScriptInterface<IBaseTree>> &Path)
{

    TArray<TScriptInterface<IBaseTree>> StaticPath = Path;
    StaticPath.Add(Target);
    TArray<TScriptInterface<IBaseTree>> DynamicPath = StaticPath;
    if (Target.GetObject() == Child.GetObject())
    {
        Path = StaticPath;
        return true;
    }
    auto children = IGetDirectChildren(Target);
    for (auto child : children)
    {
        if (IsParentOf(child, Child, DynamicPath))
        {
            Path = DynamicPath;
            return true;
        }
        DynamicPath = StaticPath;
    }
    Path = {};
    return false;
}

bool UBaseTreeLibrary::IsChildOf(const TScriptInterface<IBaseTree> &Target, const TScriptInterface<IBaseTree> &Parent, TArray<TScriptInterface<IBaseTree>> &Path)
{
    return IsParentOf(Parent, Target, Path);
}

uint8 UBaseTreeLibrary::GetDepth(const TScriptInterface<IBaseTree> &Target)
{
    if (IsRoot(Target))
        return 0;
    auto parent = IGetParentTree(Target);
    return GetDepth(parent) + 1;
}

uint8 UBaseTreeLibrary::GetMaxDepthToLeafs(const TScriptInterface<IBaseTree> &Target)
{
    uint8 max = 0;
    auto children = IGetDirectChildren(Target);
    for (auto child : children)
    {
        max = FMath::Max<uint8>(max, GetMaxDepthToLeafs(child) + 1);
    }
    return max;
}
uint8 UBaseTreeLibrary::GetMinDepthToLeafs(const TScriptInterface<IBaseTree> &Target)
{
    uint8 min = 0;
    auto children = IGetDirectChildren(Target);
    for (auto child : children)
    {
        min = FMath::Min<uint8>(min, GetMinDepthToLeafs(child) + 1);
    }
    return min;
}

TScriptInterface<IBaseTree> UBaseTreeLibrary::GetRootParent(const TScriptInterface<IBaseTree> &Target)
{
    if (IsRoot(Target))
        return Target;
    return IGetParentTree(Target);
}

TArray<TScriptInterface<IBaseTree>> UBaseTreeLibrary::GetParentsFromRoot(const TScriptInterface<IBaseTree> &Target)
{
    if (IsRoot(Target))
        return {};
    return GetParentsFromRoot(IGetParentTree(Target).GetObject()) += {Target};
}

uint8 UBaseTreeLibrary::GetSize(const TScriptInterface<IBaseTree> &Target)
{
    uint8 size = 1;
    auto children = IGetDirectChildren(Target);
    for (auto child : children)
    {
        size += GetSize(child);
    }
    return size;
}

uint8 UBaseTreeLibrary::GetLeafsSize(const TScriptInterface<IBaseTree> &Target)
{
    if (IsLeaf(Target))
        return 1;
    uint8 size = 0;
    auto children = IGetDirectChildren(Target);
    for (auto child : children)
    {
        size += GetLeafsSize(child);
    }
    return size;
}

uint8 UBaseTreeLibrary::GetTreeSizeToDepth(const TScriptInterface<IBaseTree> &Target, uint8 depth)
{

    if (depth < 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("depth = %d <0!"), depth);
    }
    if (depth == 0)
        return 1;
    if (depth == 1)
        return IGetDirectChildrenSize(Target);
    uint8 size = 1;
    auto children = IGetDirectChildren(Target);
    for (auto child : children)
    {
        size += GetTreeSizeToDepth(child, depth - 1);
    }
    return size;
}

uint8 UBaseTreeLibrary::GetTreeSizeAtDepth(const TScriptInterface<IBaseTree> &Target, uint8 depth)
{

    if (depth < 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("depth = %d <0!"), depth);
    }
    if (depth == 0)
        return 1;
    if (depth == 1)
        return IGetDirectChildrenSize(Target);
    uint8 size = 0;
    auto children = IGetDirectChildren(Target);
    for (auto child : children)
    {
        size += GetTreeSizeAtDepth(child, depth - 1);
    }
    return size;
}

TArray<TScriptInterface<IBaseTree>> UBaseTreeLibrary::GetTree(const TScriptInterface<IBaseTree> &Target)
{
    TArray<TScriptInterface<IBaseTree>> out({Target});
    auto children = IGetDirectChildren(Target);
    for (auto child : children)
    {
        out.Append(GetTree(child));
    }
    return out;
}

TArray<TScriptInterface<IBaseTree>> UBaseTreeLibrary::GetLeafs(const TScriptInterface<IBaseTree> &Target)
{
    if (IsLeaf(Target))
        return {Target};
    TArray<TScriptInterface<IBaseTree>> out;
    auto children = IGetDirectChildren(Target);
    for (auto child : children)
    {
        out.Append(GetLeafs(child));
    }
    return out;
}

TArray<TScriptInterface<IBaseTree>> UBaseTreeLibrary::GetChildrenToDepth(const TScriptInterface<IBaseTree> &Target, uint8 depth)
{

    if (depth < 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("depth = %d <0!"), depth);
    }
    if (depth == 0)
        return {Target};
    if (depth == 1)
        return IGetDirectChildren(Target);
    TArray<TScriptInterface<IBaseTree>> out({Target});
    auto children = IGetDirectChildren(Target);
    for (auto child : children)
    {
        out.Append(GetChildrenToDepth(child, depth - 1));
    }
    return out;
}

TArray<TScriptInterface<IBaseTree>> UBaseTreeLibrary::GetChildrenAtDepth(const TScriptInterface<IBaseTree> &Target, uint8 depth)
{
    if (depth < 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("depth = %d <0!"), depth);
    }
    if (depth == 0)
        return {Target};
    if (depth == 1)
        return IGetDirectChildren(Target);
    TArray<TScriptInterface<IBaseTree>> out;
    auto children = IGetDirectChildren(Target);
    for (auto child : children)
    {
        out.Append(GetChildrenAtDepth(child, depth - 1));
    }
    return out;
}
