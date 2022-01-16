#include "Libraries/Tree/DynamicNameMapTree.h"

TKishiScriptInterface<IDynamicNameMapTree> UDynamicNameMapTreeLibrary::ICreateTree(const TKishiScriptInterface<IDynamicNameMapTree> &Target)
{
    checkf(Target, TEXT("Error : Target is NULL or does not implement interface IDynamicMapTree\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    TKishiScriptInterface<IDynamicNameMapTree> NewTree = IDynamicNameMapTree::Execute_CreateTree(Target);
    checkf(NewTree, TEXT("Error : NewTree is NULL or does not implement interface IDynamicMapTree\n GetObject: %d\n GetInterface: %d"), NewTree.GetObject(), NewTree.GetInterface());
    return NewTree;
}
bool UDynamicNameMapTreeLibrary::IAddChildAt(const TKishiScriptInterface<IDynamicNameMapTree> &Target,FName key, const TKishiScriptInterface<IDynamicNameMapTree> &ChildToAdd)
{
    checkf(Target, TEXT("Error : Target is NULL or does not implement interface IDynamicMapTree\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    checkf(ChildToAdd, TEXT("Error : ChildToAdd is NULL or does not implement interface IDynamicMapTree\n GetObject: %d\n GetInterface: %d"), ChildToAdd.GetObject(), ChildToAdd.GetInterface());
    checkf(!IsDirectParentOf(Target, ChildToAdd), TEXT("Error : ChildToAdd is already a child of Target\n Target: %d\n Parent of ChildToAdd: %d"), Target.GetObject(), IGetParentTree(ChildToAdd).GetObject());
    auto Result = IDynamicNameMapTree::Execute_AddChildAt(Target,key, ChildToAdd);
    checkCode(
        auto isParentOf = IsDirectParentOf(Target, ChildToAdd);
        if (Result) {
            checkf(IsDirectParentOf(Target, ChildToAdd), TEXT("Error : AddChild returned true but ChildToAdd is not a child of Target\n Target: %d\n Parent of ChildToAdd: %d"), Target.GetObject(), IGetParentTree(ChildToAdd).GetObject());
        } else {
            checkf(!IsDirectParentOf(Target, ChildToAdd), TEXT("Error : AddChild returned false but ChildToAdd is a child of Target\n Target: %d\n Parent of ChildToAdd: %d"), Target.GetObject(), IGetParentTree(ChildToAdd).GetObject());
        });
    return Result;
}

bool UDynamicNameMapTreeLibrary::IRemoveChild(const TKishiScriptInterface<IDynamicNameMapTree> &Target, const TKishiScriptInterface<IDynamicNameMapTree> &ChildToRemove)
{
    checkf(Target, TEXT("Error : Target is NULL or does not implement interface IDynamicMapTree\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    checkf(ChildToRemove, TEXT("Error : ChildToRemove is NULL or does not implement interface IDynamicMapTree\n GetObject: %d\n GetInterface: %d"), ChildToRemove.GetObject(), ChildToRemove.GetInterface());
    checkf(IsDirectParentOf(Target, ChildToRemove), TEXT("Error : ChildToRemove is not a child of Target\n Target: %d\n Parent of ChildToRemove: %d"), Target.GetObject(), IGetParentTree(ChildToRemove).GetObject());
    auto Result = IDynamicNameMapTree::Execute_RemoveChild(Target, ChildToRemove);
    checkCode(
        auto isParentOf = IsDirectParentOf(Target, ChildToRemove);
        if (Result) {
            checkf(!IsDirectParentOf(Target, ChildToRemove), TEXT("Error : RemoveChild returned true but ChildToRemove is still a child of Target\n Target: %d\n Parent of ChildToRemove: %d"), Target.GetObject(), IGetParentTree(ChildToRemove).GetObject());
        } else checkf(IsDirectParentOf(Target, ChildToRemove), TEXT("Error : RemoveChild returned false but ChildToRemove is not a child of Target\n Target: %d\n Parent of ChildToRemove: %d"), Target.GetObject(), IGetParentTree(ChildToRemove).GetObject()););
    return Result;
}