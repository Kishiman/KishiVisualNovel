#include "Libraries/Tree/ArrayTree.h"
#include "Links/Tree/ArrayTree.h"
#include "Macros/Interface.h"

TKishiScriptInterface<IArrayTree> UArrayTreeLibrary::IGetChild(TKishiScriptInterface<IArrayTree> Target, uint8 index)
{
    checkf(Target, TEXT("Error : Target is NULL or does not implement interface IArrayTree\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    auto size = IGetDirectChildrenSize(Target);
    checkf(index < size, TEXT("Error : index out of range\n Children Size: %d\n Index: %d"), size, index);
    return IArrayTree::Execute_GetChild(Target, index);
}

TScriptInterface<IArrayTree> UArrayTreeLibrary::IGetChild_Default(const TScriptInterface<IArrayTree> &Target, uint8 index)
{
    auto children = IGetDirectChildren(Target.GetObject());
    return children[index].GetObject();
}

bool UArrayTreeLibrary::IsFirst(const TScriptInterface<IArrayTree> &Target)
{
    if (IsRoot(Target.GetObject()))
    {
        return true;
    }
    TScriptInterface<IArrayTree> parent = IGetParentTree(Target.GetObject()).GetObject();
    return GetFirstChild(parent).GetObject() == Target.GetObject();
}

bool UArrayTreeLibrary::IsLast(const TScriptInterface<IArrayTree> &Target)
{
    if (IsRoot(Target.GetObject()))
    {
        return true;
    }
    TScriptInterface<IArrayTree> parent = IGetParentTree(Target.GetObject()).GetObject();
    return GetLastChild(parent).GetObject() == Target.GetObject();
}

uint8 UArrayTreeLibrary::GetIndex(const TScriptInterface<IArrayTree> &Target)
{
    if (IsRoot(Target.GetObject()))
    {
        return 0;
    }
    TScriptInterface<IArrayTree> parent = IGetParentTree(Target);
    auto size = IGetDirectChildrenSize(parent);
    for (uint8 i = 0; i < size; ++i)
    {
        auto it = IGetChild(parent, i);
        if (it == Target)
        {
            return i;
        }
    }
    checkNoEntry() return size;
}

uint8 UArrayTreeLibrary::GetIndexInParent(const TScriptInterface<IArrayTree> &Target, uint8 depth)
{
    if (IsRoot(Target.GetObject()))
    {
        return 0;
    }
    if (depth == 0)
    {
        return 0;
    }
    auto parent = IGetParentTree(Target);
    uint8 idxInParent = GetIndexInParent(parent, depth - 1);
    uint8 targetIdx = GetIndex(Target);
    for (uint8 i = 0; i < targetIdx; ++i)
    {
        auto it = IGetChild(parent, i);
        idxInParent += GetSize(it);
    }
    return idxInParent + 1;
    //Not Optimized
}

uint8 UArrayTreeLibrary::GetIndexInRoot(const TScriptInterface<IArrayTree> &Target)
{
    auto depth = GetDepth(Target.GetObject());
    return GetIndexInParent(Target, depth);
}

uint8 UArrayTreeLibrary::GetLeafIndex(const TScriptInterface<IArrayTree> &Target)
{
    if (IsRoot(Target.GetObject()))
    {
        return 0;
    }
    TScriptInterface<IArrayTree> parent = IGetParentTree(Target.GetObject()).GetObject();
    uint8 leafIndex = GetLeafIndex(parent);
    uint8 targetIdx = GetIndex(Target);
    for (uint8 i = 0; i < targetIdx; ++i)
    {
        TScriptInterface<IArrayTree> it = IGetChild(parent, i);
        if (it.GetObject())
            leafIndex += GetLeafsSize(it.GetObject());
    }
    return leafIndex;
}
TScriptInterface<IArrayTree> UArrayTreeLibrary::GetFirstChild(const TScriptInterface<IArrayTree> &Target, bool bReturnTargetIfLeaf)
{
    if (IsLeaf(Target.GetObject()))
    {
        if (bReturnTargetIfLeaf)
            return Target;
        return NULL;
    }
    return IGetChild(Target, 0);
}

TScriptInterface<IArrayTree> UArrayTreeLibrary::GetLastChild(const TScriptInterface<IArrayTree> &Target, bool bReturnTargetIfLeaf)
{
    if (IsLeaf(Target.GetObject()))
    {
        if (bReturnTargetIfLeaf)
            return Target;
        return NULL;
    }
    return IGetChild(Target, IGetDirectChildrenSize(Target.GetObject()) - 1);
}

TScriptInterface<IArrayTree> UArrayTreeLibrary::GetNotFirstParent(const TScriptInterface<IArrayTree> &Target, uint8 &OutDistance)
{
    TScriptInterface<IArrayTree> it = Target;
    OutDistance = 0;
    while (it.GetObject() && IsFirst(it))
    {
        it = IGetParentTree(it.GetObject()).GetObject();
        ++(OutDistance);
    }
    return it;
}
TScriptInterface<IArrayTree> UArrayTreeLibrary::GetNotLastParent(const TScriptInterface<IArrayTree> &Target, uint8 &OutDistance)
{
    TScriptInterface<IArrayTree> it = Target;
    OutDistance = 0;
    while (it.GetObject() && IsLast(it))
    {
        it = IGetParentTree(it.GetObject()).GetObject();
        ++(OutDistance);
    }
    return it;
}

TScriptInterface<IArrayTree> UArrayTreeLibrary::GetNext(const TScriptInterface<IArrayTree> &Target)
{
    if (!IsLeaf(Target.GetObject()))
    {
        return GetFirstChild(Target);
    }
    uint8 foo;
    TScriptInterface<IArrayTree> notLast = GetNotLastParent(Target, foo);
    if (notLast.GetObject())
    {
        GetNextDirectSibling(notLast);
    }
    return NULL;
}

TScriptInterface<IArrayTree> UArrayTreeLibrary::GetPrevious(const TScriptInterface<IArrayTree> &Target)
{
    uint8 foo;
    TScriptInterface<IArrayTree> notFirst = GetNotFirstParent(Target, foo);
    if (notFirst.GetObject())
        return GetLastLeaf(GetPreviousDirectSibling(notFirst));
    return NULL;
}

TScriptInterface<IArrayTree> UArrayTreeLibrary::GetNextDirectSibling(const TScriptInterface<IArrayTree> &Target)
{
    if (IsLast(Target))
        return NULL;
    TScriptInterface<IArrayTree> parent = IGetParentTree(Target.GetObject()).GetObject();
    uint8 idx = GetIndex(Target);
    return IGetChild(parent, idx + 1);
}

TScriptInterface<IArrayTree> UArrayTreeLibrary::GetPreviousDirectSibling(const TScriptInterface<IArrayTree> &Target)
{
    if (IsFirst(Target))
        return NULL;
    TScriptInterface<IArrayTree> parent = IGetParentTree(Target.GetObject()).GetObject();
    uint8 idx = GetIndex(Target);
    if (idx > 0)
    {
        return IGetChild(parent, idx - 1);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Array Tree not first but returned index=0"));
    }
    return NULL;
}

TScriptInterface<IArrayTree> UArrayTreeLibrary::GetFirstLeaf(const TScriptInterface<IArrayTree> &Target)
{
    if (IsLeaf(Target.GetObject()))
    {
        return Target;
    }
    return GetFirstLeaf(GetFirstChild(Target));
}

TScriptInterface<IArrayTree> UArrayTreeLibrary::GetLastLeaf(const TScriptInterface<IArrayTree> &Target)
{
    if (IsLeaf(Target.GetObject()))
    {
        return Target;
    }
    return GetLastLeaf(GetLastChild(Target));
}

TScriptInterface<IArrayTree> UArrayTreeLibrary::GetNextLeaf(const TScriptInterface<IArrayTree> &Target)
{
    if (IsLeaf(Target.GetObject()))
    {
        TScriptInterface<IArrayTree> next = GetNext(Target);
        if (next.GetObject())
        {
            return GetFirstLeaf(next);
        }
        return NULL;
    }
    return GetNextLeaf(GetFirstLeaf(Target));
}

TScriptInterface<IArrayTree> UArrayTreeLibrary::GetPreviousLeaf(const TScriptInterface<IArrayTree> &Target)
{
    if (IsLeaf(Target.GetObject()))
    {
        TScriptInterface<IArrayTree> prev = GetPrevious(Target);
        if (prev.GetObject())
        {
            return GetLastLeaf(prev);
        }
        return NULL;
    }
    uint8 foo;
    TScriptInterface<IArrayTree> notFirst = GetNotFirstParent(Target, foo);
    if (notFirst.GetObject())
    {
        return GetPrevious(notFirst);
    }
    return NULL;
}

TScriptInterface<IArrayTree> UArrayTreeLibrary::GetFirstAtDepth(const TScriptInterface<IArrayTree> &Target, uint8 Depth)
{
    if (Depth == 0)
    {
        return Target;
    }
    uint8 size = IGetDirectChildrenSize(Target.GetObject());
    TScriptInterface<IArrayTree> firstAtDepth;
    for (uint8 i = 0; i < size; ++i)
    {
        firstAtDepth = GetFirstAtDepth(IGetChild(Target, i).GetObject(), Depth - 1);
        if (firstAtDepth.GetObject())
            return firstAtDepth;
    }
    return NULL;
}
TScriptInterface<IArrayTree> UArrayTreeLibrary::GetLastAtDepth(const TScriptInterface<IArrayTree> &Target, uint8 Depth)
{
    if (Depth == 0)
    {
        return Target;
    }
    uint8 size = IGetDirectChildrenSize(Target.GetObject());
    TScriptInterface<IArrayTree> lastAtDepth;
    for (uint8 i = size - 1; i > -1; --i)
    {
        lastAtDepth = GetLastAtDepth(IGetChild(Target, i).GetObject(), Depth - 1);
        if (lastAtDepth.GetObject())
            return lastAtDepth;
    }
    return NULL;
}

TScriptInterface<IArrayTree> UArrayTreeLibrary::GetNextAtSameDepth(const TScriptInterface<IArrayTree> &Target)
{
    uint8 distance;
    auto notLastParent = GetNotLastParent(Target, distance);
    while (notLastParent.GetObject())
    {
        auto nextAtDepth = GetFirstAtDepth(GetNextDirectSibling(notLastParent), distance);
        if (nextAtDepth.GetObject())
            return nextAtDepth;
        notLastParent = GetNextAtSameDepth(notLastParent);
    }
    return NULL;
}

TScriptInterface<IArrayTree> UArrayTreeLibrary::GetPreviousAtSameDepth(const TScriptInterface<IArrayTree> &Target)
{
    uint8 distance;
    auto notFirstParent = GetNotFirstParent(Target, distance);
    while (notFirstParent.GetObject())
    {
        auto prevAtDepth = GetLastAtDepth(GetPreviousDirectSibling(notFirstParent), distance);
        if (prevAtDepth.GetObject())
            return prevAtDepth;
        notFirstParent = GetPreviousAtSameDepth(notFirstParent);
    }
    return NULL;
}

TArray<uint8> UArrayTreeLibrary::GetPath(const TScriptInterface<IArrayTree> &Target)
{
    if (IsRoot(Target.GetObject()))
    {
        return {0};
    }
    return GetPath(IGetParentTree(Target.GetObject()).GetObject()) += {GetIndex(Target)};
}

bool UArrayTreeLibrary::IsDirectParentOfWithIndex(const TScriptInterface<IArrayTree> &Target, const TScriptInterface<IArrayTree> &Child, uint8 &IndexOfChild)
{
    IndexOfChild = GetIndex(Child);
    return IsDirectParentOf(Target.GetObject(), Child.GetObject());
}

bool UArrayTreeLibrary::IsDirectChildOfWithIndex(const TScriptInterface<IArrayTree> &Target, const TScriptInterface<IArrayTree> &Parent, uint8 &IndexOfChild)
{
    return IsDirectParentOfWithIndex(Parent, Target, IndexOfChild);
}

bool UArrayTreeLibrary::IsParentOfWithPath(const TScriptInterface<IArrayTree> &Target, const TScriptInterface<IArrayTree> &Child, TArray<uint8> &Path)
{
    return false;
}

bool UArrayTreeLibrary::IsChildOfWithPath(const TScriptInterface<IArrayTree> &Target, const TScriptInterface<IArrayTree> &Parent, TArray<uint8> &Path)
{
    if (Target.GetObject() == Parent.GetObject())
    {
        Path = {};
        return true;
    }
    if (IsRoot(Target.GetObject()))
    {
        Path = {};
        return false;
    }
    if (IsChildOfWithPath(IGetParentTree(Target.GetObject()).GetObject(), Parent, Path))
    {
        Path.Add(GetIndex(Target));
        return true;
    }
    return false;
}

TScriptInterface<IArrayTree> UArrayTreeLibrary::GetChildAtPath(const TScriptInterface<IArrayTree> &Target, TArray<uint8> Path)
{
    if (Path.Num() == 0)
        return Target;
    auto child = IGetChild(Target, Path[0]);
    Path.RemoveAt(0);
    if (child.GetObject())
        return GetChildAtPath(child, Path);
    return NULL;
}

TScriptInterface<IArrayTree> UArrayTreeLibrary::GetChildAtIndex(const TScriptInterface<IArrayTree> &Target, uint8 Index)
{
    if (Index == 0)
        return Target;
    if (IsLeaf(Target.GetObject()))
        return NULL;
    uint8 size = IGetDirectChildrenSize(Target.GetObject());
    uint8 PreviousIndex = 1;
    for (uint8 i = 0; i < size; ++i)
    {
        auto child = IGetChild(Target, i);
        auto target = GetChildAtIndex(child, Index - PreviousIndex);
        if (target.GetObject())
        {
            return target;
        }
        PreviousIndex += GetSize(child.GetObject());
    }
    return NULL;
}

TScriptInterface<IArrayTree> UArrayTreeLibrary::GetChildAtRootIndex(const TScriptInterface<IArrayTree> &Target, uint8 Index)
{
    uint8 myIndex = GetIndexInRoot(Target);
    if (Index < myIndex)
        return NULL;
    return GetChildAtIndex(Target, Index - myIndex);
}