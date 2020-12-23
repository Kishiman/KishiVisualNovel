#include "Libraries/Tree/DynamicTree.h"

TKishiScriptInterface<IDynamicTree> UDynamicTreeLibrary::ICreateTree(const TKishiScriptInterface<IDynamicTree> &Target)
{
    checkf(Target, TEXT("Error : Target is NULL or does not implement interface IArrayTree\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    TKishiScriptInterface<IDynamicTree> NewTree = IDynamicTree::Execute_CreateTree(Target);
    checkf(NewTree, TEXT("Error : NewTree is NULL or does not implement interface IArrayTree\n GetObject: %d\n GetInterface: %d"), NewTree.GetObject(), NewTree.GetInterface());
    return NewTree;
}
bool UDynamicTreeLibrary::IAddChild(const TKishiScriptInterface<IDynamicTree> &Target, const TKishiScriptInterface<IDynamicTree> &ChildToAdd)
{
    checkf(Target, TEXT("Error : Target is NULL or does not implement interface IArrayTree\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    checkf(ChildToAdd, TEXT("Error : ChildToAdd is NULL or does not implement interface IArrayTree\n GetObject: %d\n GetInterface: %d"), ChildToAdd.GetObject(), ChildToAdd.GetInterface());
    checkf(!IsDirectParentOf(Target, ChildToAdd), TEXT("Error : ChildToAdd is already a child of Target\n Target: %d\n Parent of ChildToAdd: %d"), Target.GetObject(), IGetParentTree(ChildToAdd).GetObject());
    auto Result = IDynamicTree::Execute_AddChild(Target, ChildToAdd);
    checkCode(
        auto isParentOf = IsDirectParentOf(Target, ChildToAdd);
        if (Result) {
            checkf(IsDirectParentOf(Target, ChildToAdd), TEXT("Error : AddChild returned true but ChildToAdd is not a child of Target\n Target: %d\n Parent of ChildToAdd: %d"), Target.GetObject(), IGetParentTree(ChildToAdd).GetObject());
        } else {
            checkf(!IsDirectParentOf(Target, ChildToAdd), TEXT("Error : AddChild returned false but ChildToAdd is a child of Target\n Target: %d\n Parent of ChildToAdd: %d"), Target.GetObject(), IGetParentTree(ChildToAdd).GetObject());
        });
    return Result;
}
bool UDynamicTreeLibrary::IRemoveChild(const TKishiScriptInterface<IDynamicTree> &Target, const TKishiScriptInterface<IDynamicTree> &ChildToRemove)
{
    checkf(Target, TEXT("Error : Target is NULL or does not implement interface IArrayTree\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    checkf(ChildToRemove, TEXT("Error : ChildToRemove is NULL or does not implement interface IArrayTree\n GetObject: %d\n GetInterface: %d"), ChildToRemove.GetObject(), ChildToRemove.GetInterface());
    checkf(IsDirectParentOf(Target, ChildToRemove), TEXT("Error : ChildToRemove is not a child of Target\n Target: %d\n Parent of ChildToRemove: %d"), Target.GetObject(), IGetParentTree(ChildToRemove).GetObject());
    auto Result = IDynamicTree::Execute_RemoveChild(Target, ChildToRemove);
    checkCode(
        auto isParentOf = IsDirectParentOf(Target, ChildToRemove);
        if (Result) {
            checkf(!IsDirectParentOf(Target, ChildToRemove), TEXT("Error : RemoveChild returned true but ChildToRemove is still a child of Target\n Target: %d\n Parent of ChildToRemove: %d"), Target.GetObject(), IGetParentTree(ChildToRemove).GetObject());
        } else checkf(IsDirectParentOf(Target, ChildToRemove), TEXT("Error : RemoveChild returned false but ChildToRemove is not a child of Target\n Target: %d\n Parent of ChildToRemove: %d"), Target.GetObject(), IGetParentTree(ChildToRemove).GetObject()););
    return Result;
}

bool UDynamicTreeLibrary::AddNewChild(const TScriptInterface<IDynamicTree> &Target, TScriptInterface<IDynamicTree> &ChildAdded)
{
    auto childAdded = ICreateTree(Target);
    if (IAddChild(Target, childAdded))
        return true;
    ChildAdded = NULL;
    return false;
}

bool UDynamicTreeLibrary::AddNewChildToParent(const TScriptInterface<IDynamicTree> &Target, TScriptInterface<IDynamicTree> &ChildAdded, uint8 parentIndex)
{
    if (parentIndex == 0)
    {
        return AddNewChild(Target, ChildAdded);
    }
    if (IsRoot(Target.GetObject()))
    {
        return false;
    }
    return AddNewChildToParent(IGetParentTree(Target), ChildAdded, parentIndex - 1);
}

bool UDynamicTreeLibrary::AddNewChilden(const TScriptInterface<IDynamicTree> &Target, TArray<TScriptInterface<IDynamicTree>> &ChildrenAdded, uint8 count)
{
    for (uint8 it = 0; it < count; ++it)
    {
        TKishiScriptInterface<IDynamicTree> newChild;
        if (AddNewChild(Target, newChild))
        {
            ChildrenAdded.Add(newChild);
        }
        else
        {
            ChildrenAdded.Empty();
            return false;
        }
    }
    return true;
}

bool UDynamicTreeLibrary::AddNewChildrenToParent(const TScriptInterface<IDynamicTree> &Target, TArray<TScriptInterface<IDynamicTree>> &TreesAdded, uint8 count, uint8 parentIndex)
{
    if (parentIndex == 0)
    {
        return AddNewChilden(Target, TreesAdded, count);
    }
    if (IsRoot(Target.GetObject()))
    {
        return false;
    }
    return AddNewChildrenToParent(IGetParentTree(Target), TreesAdded, count, parentIndex - 1);
}

bool UDynamicTreeLibrary::JoinTwoTrees(const TScriptInterface<IDynamicTree> &Target, TScriptInterface<IDynamicTree> &AnotherTree, TScriptInterface<IDynamicTree> &NewParentTree)
{
    auto parent = IGetParentTree(Target);
    NewParentTree = ICreateTree(Target);
    if (NewParentTree.GetObject())
    {
        if (IAddChild(NewParentTree, Target))
        {
            if (IAddChild(NewParentTree, AnotherTree))
            {
                return true;
            }
            else if (IRemoveChild(NewParentTree, Target) && IAddChild(parent, Target))
            {
                NewParentTree = NULL;
                return true;
            }
            //CheckNoEntry();
        }
        return false;
    }
    return false;
}

bool UDynamicTreeLibrary::JoinTrees(const TArray<TScriptInterface<IDynamicTree>> &Trees, TScriptInterface<IDynamicTree> &NewParentTree)
{
    //auto parent=IGetParentTree(Target);
    if (Trees.Num() == 0)
    {
        return false;
    }
    auto Target = Trees[0];
    NewParentTree = ICreateTree(Target);
    if (NewParentTree.GetObject())
    {
        for (auto &&tree : Trees)
        {
            if (!IAddChild(NewParentTree, tree))
            {
                UE_LOG(LogTemp, Warning, TEXT("JoinTrees failed midway, unexpected behaviour"));
                return false;
            }
        }
        return true;
    }
    return false;
}

bool UDynamicTreeLibrary::RemoveFromParent(const TScriptInterface<IDynamicTree> &Target)
{
    if (IsRoot(Target.GetObject()))
    {
        return false;
    }
    return IRemoveChild(IGetParentTree(Target), Target);
}

bool UDynamicTreeLibrary::RemoveParentFromParent(const TScriptInterface<IDynamicTree> &Target, uint8 ParentIndex)
{
    if (ParentIndex == 0)
    {
        return false;
    }
    if (IsRoot(Target.GetObject()))
    {
        return false;
    }
    if (ParentIndex == 1)
    {
        return RemoveFromParent(Target);
    }
    return RemoveParentFromParent(IGetParentTree(Target), ParentIndex - 1);
}

bool UDynamicTreeLibrary::RemoveDirectChildren(const TScriptInterface<IDynamicTree> &Target, const TArray<TScriptInterface<IDynamicTree>> &DirectChildrenToRemove)
{
    for (TKishiScriptInterface<IDynamicTree> &&child : DirectChildrenToRemove)
    {
        if (!IRemoveChild(Target, child))
            return false;
    }
    return true;
}

bool UDynamicTreeLibrary::RemoveChildren(const TScriptInterface<IDynamicTree> &Target, const TArray<TScriptInterface<IDynamicTree>> &ChildrenToRemove)
{
    for (TKishiScriptInterface<IDynamicTree> &&child : ChildrenToRemove)
    {
        if (!(IsParentOf(Target.GetObject(), child) && RemoveFromParent(child)))
            return false;
    }
    return true;
}

bool UDynamicTreeLibrary::RemoveAllDirectChildren(const TScriptInterface<IDynamicTree> &Target, TArray<TScriptInterface<IDynamicTree>> &DirectChildrenRemoved)
{
    auto children = IGetDirectChildren(Target);
    for (TKishiScriptInterface<IDynamicTree> &&child : children)
    {
        if (!IRemoveChild(Target, child))
            return false;
        DirectChildrenRemoved.Add(child);
    }
    return true;
}

bool UDynamicTreeLibrary::RemoveAllChildren(const TScriptInterface<IDynamicTree> &Target, TArray<TScriptInterface<IDynamicTree>> &ChildrenRemoved)
{
    auto children = IGetDirectChildren(Target);
    for (TKishiScriptInterface<IDynamicTree> &&child : children)
    {
        if (!IRemoveChild(Target, child))
            return false;
        ChildrenRemoved.Add(child);
    }
    return true;
}

bool UDynamicTreeLibrary::RemoveDirectLeafs(const TScriptInterface<IDynamicTree> &Target, TArray<TScriptInterface<IDynamicTree>> &DirectLeafsRemoved)
{
    auto DirectLeafs = GetDirectLeafs(Target.GetObject());
    for (TKishiScriptInterface<IDynamicTree> &&leaf : DirectLeafs)
    {
        if (!IRemoveChild(Target, leaf))
            return false;
        DirectLeafsRemoved.Add(leaf);
    }
    return true;
}

bool UDynamicTreeLibrary::RemoveDirectNonLeafs(const TScriptInterface<IDynamicTree> &Target, TArray<TScriptInterface<IDynamicTree>> &DirectNonLeafsRemoved)
{
    auto DirectNonLeafs = GetDirectNonLeafs(Target.GetObject());
    for (TKishiScriptInterface<IDynamicTree> &&nonLeaf : DirectNonLeafs)
    {
        if (!IRemoveChild(Target, nonLeaf))
            return false;
        DirectNonLeafsRemoved.Add(nonLeaf);
    }
    return true;
}

bool UDynamicTreeLibrary::MakeAllDirectChildrenLeafs(const TScriptInterface<IDynamicTree> &Target, TArray<TScriptInterface<IDynamicTree>> &ChildrenRemoved)
{
    auto DirectNonLeafs = GetDirectNonLeafs(Target.GetObject());
    for (TKishiScriptInterface<IDynamicTree> &&nonLeaf : DirectNonLeafs)
    {
        if (!RemoveAllDirectChildren(nonLeaf, ChildrenRemoved))
            return false;
    }
    return true;
}