#include "Libraries/Tree/BaseTree.h"
#include "Templates/KishiScriptInterface.h"
#include "Macros/Interface.h"
#include "Algo/Reverse.h"


uint8 IBaseTree::GetDirectChildrenSize_Implementation() const{
    auto children = GetDirectChildren_Implementation();
    return children.Num();
}

TKishiScriptInterface<IBaseTree> UBaseTreeLibrary::IGetParentTree(TKishiScriptInterface<IBaseTree> Target)
{
    checkf(Target.GetObject() != NULL, TEXT("Error : Target is NULL or does not implement Interface IBaseTree\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    checkCode(
        auto parent = IBaseTree::Execute_GetParentTree(Target.GetObject());
        while (parent.GetObject()) {
            checkf(Target.GetObject() != parent.GetObject(), TEXT("Error : Tree Has Cycle, target is parent of itself"));
            parent = IBaseTree::Execute_GetParentTree(parent.GetObject());
        });
    checkCode(

        auto parent = IBaseTree::Execute_GetParentTree(Target.GetObject());
        if (parent.GetObject()) {
            auto children = IBaseTree::Execute_GetDirectChildren(parent.GetObject());
            checkf(children.Contains(Target), TEXT("Error : Broken Tree, KishiTarget not in parent children"))
        });
    auto parent = IBaseTree::Execute_GetParentTree(Target.GetObject());
    return parent;
}

TArray<TScriptInterface<IBaseTree>> UBaseTreeLibrary::IGetDirectChildren(TKishiScriptInterface<IBaseTree> Target)
{
    checkf(Target.GetObject() != NULL, TEXT("Error : Target is NULL or does not implement Interface IBaseTree\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    checkCode(
        auto children = IBaseTree::Execute_GetDirectChildren(Target.GetObject());
        for (auto &&child
             : children) {
            checkf(IGetParentTree(child) == Target, TEXT("Error : Broken Tree, child not connected to target"));
        });
    auto children = IBaseTree::Execute_GetDirectChildren(Target.GetObject());
    return children;
}

uint8 UBaseTreeLibrary::IGetDirectChildrenSize(TKishiScriptInterface<IBaseTree> Target)
{
    checkf(Target.GetObject() != NULL, TEXT("Error : Target is NULL or does not implement Interface IBaseTree\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    uint8 childrenSize = IBaseTree::Execute_GetDirectChildrenSize(Target.GetObject());
    checkCode(
        auto children = IGetDirectChildren(Target);
        checkf(childrenSize == children.Num(), TEXT("Error : Broken Tree, children size don't match children returned")););
    return childrenSize;
}

uint8 UBaseTreeLibrary::IGetDirectChildrenSize_Default(const TScriptInterface<IBaseTree> &Target)
{
    auto children = IGetDirectChildren(Target);
    return children.Num();
}

bool UBaseTreeLibrary::IsLeaf(const TScriptInterface<IBaseTree> &Target)
{
    auto childrenSize = IGetDirectChildrenSize(Target);
    return childrenSize == 0;
}

bool UBaseTreeLibrary::IsRoot(const TScriptInterface<IBaseTree> &Target)
{
    auto parent = IGetParentTree(Target);
    return parent.GetObject() == NULL;
}

bool UBaseTreeLibrary::IsDirectParentOf(const TScriptInterface<IBaseTree> &Target, const TScriptInterface<IBaseTree> &Child)
{
    auto parent = IGetParentTree(Child);
    return (Target.GetObject() != NULL) && parent == Target;
}

bool UBaseTreeLibrary::IsDirectChildOf(const TScriptInterface<IBaseTree> &Target, const TScriptInterface<IBaseTree> &Parent)
{
    return IsDirectParentOf(Parent, Target);
}

bool UBaseTreeLibrary::IsParentOf(const TScriptInterface<IBaseTree> &Target, const TScriptInterface<IBaseTree> &Child)
{
    if (Target.GetObject() == Child.GetObject())
    {
        return true;
    }
    auto children = IGetDirectChildren(Target);
    for (auto child : children)
    {
        if (IsParentOf(child, Child))
        {
            return true;
        }
    }
    return false;
}

bool UBaseTreeLibrary::IsChildOf(const TScriptInterface<IBaseTree> &Target, const TScriptInterface<IBaseTree> &Parent)
{
    return IsParentOf(Parent, Target);
}

bool UBaseTreeLibrary::IsParentOfWithNodes(const TScriptInterface<IBaseTree> &Target, const TScriptInterface<IBaseTree> &Child, TArray<TScriptInterface<IBaseTree>> &Nodes)
{
    Nodes.Empty();
    if (!IsParentOf(Target, Child))
    {
        return false;
    }
    auto parent = Child;
    while (parent != Target)
    {
        Nodes.Add(parent);
        parent = IGetParentTree(parent);
    }
    Algo::Reverse(Nodes);
    return true;
}

bool UBaseTreeLibrary::IsChildOfWithNodes(const TScriptInterface<IBaseTree> &Target, const TScriptInterface<IBaseTree> &Parent, TArray<TScriptInterface<IBaseTree>> &Nodes)
{
    return IsParentOfWithNodes(Parent, Target, Nodes);
}
bool UBaseTreeLibrary::IsParentOfWithDistance(const TScriptInterface<IBaseTree> &Target, const TScriptInterface<IBaseTree> &Child, uint8 &Distance)
{
    Distance = 0;
    if (!IsParentOf(Target, Child))
    {
        return false;
    }
    auto parent = Child;
    while (parent != Target)
    {
        ++Distance;
        parent = IGetParentTree(parent);
    }
    return true;
}

bool UBaseTreeLibrary::IsChildOfWithDistance(const TScriptInterface<IBaseTree> &Target, const TScriptInterface<IBaseTree> &Parent, uint8 &Distance)
{
    return IsParentOfWithDistance(Parent, Target, Distance);
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
TArray<TScriptInterface<IBaseTree>> UBaseTreeLibrary::GetDirectLeafs(const TScriptInterface<IBaseTree> &Target)
{
    if (IsLeaf(Target))
        return {};
    auto children = IGetDirectChildren(Target);
    TArray<TScriptInterface<IBaseTree>> out;
    out.Reserve(children.Num());
    for (auto child : children)
    {
        if (IsLeaf(child))
            out.Add(child);
    }
    return out;
}
TArray<TScriptInterface<IBaseTree>> UBaseTreeLibrary::GetDirectNonLeafs(const TScriptInterface<IBaseTree> &Target)
{
    if (IsLeaf(Target))
        return {};
    auto children = IGetDirectChildren(Target);
    TArray<TScriptInterface<IBaseTree>> out;
    out.Reserve(children.Num());
    for (auto child : children)
    {
        if (!IsLeaf(child))
            out.Add(child);
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
