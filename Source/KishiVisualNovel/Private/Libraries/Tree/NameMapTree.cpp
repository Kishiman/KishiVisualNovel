#include "Libraries/Tree/NameMapTree.h"
#include "Macros/Interface.h"

//IArrayTree overrides
TScriptInterface<IArrayTree> INameMapTree::GetChild_Implementation(uint8 index) const
{
    auto names = this->GetNames_Implementation();
    auto name = names[index];
    return this->GetChildByName_Implementation(name).GetObject();
}

TScriptInterface<IArrayTree> UNameMapTreeImplementation::IGetChild_Default(const TScriptInterface<INameMapTree> &Target, uint8 index)
{
    auto names = INameMapTree::Execute_GetNames(Target.GetObject());
    auto name = names[index];
    return INameMapTree::Execute_GetChildByName(Target.GetObject(), name).GetObject();
}

//IBaseTree overrides
TArray<TScriptInterface<IBaseTree>> INameMapTree::GetDirectChildren_Implementation() const
{
    auto names = this->GetNames_Implementation();
    TArray<TScriptInterface<IBaseTree>> DirectChildren;
    DirectChildren.Reserve(names.Num());
    for (auto &&name : names)
    {
        TKishiScriptInterface<IArrayTree> child = this->GetChildByName_Implementation(name);
        DirectChildren.Add(child);
    }
    return DirectChildren;
}

TArray<TScriptInterface<IBaseTree>> UNameMapTreeImplementation::IGetDirectChildren_Default(const TScriptInterface<INameMapTree> &Target)
{
    auto names = INameMapTree::Execute_GetNames(Target.GetObject());
    TArray<TScriptInterface<IBaseTree>> DirectChildren;
    DirectChildren.Reserve(names.Num());
    for (auto &&name : names)
    {
        TKishiScriptInterface<IBaseTree> child = INameMapTree::Execute_GetChildByName(Target.GetObject(), name);
        DirectChildren.Add(child);
    }
    return DirectChildren;
}

uint8 INameMapTree::GetDirectChildrenSize_Implementation() const
{
    return this->GetNames_Implementation().Num();
}

uint8 UNameMapTreeImplementation::IGetDirectChildrenSize_Default(const TScriptInterface<INameMapTree> &Target)
{
    return INameMapTree::Execute_GetNames(Target.GetObject()).Num();
}
//Proxy Functions
TKishiScriptInterface<INameMapTree> UNameMapTreeLibrary::IGetChildByName(const TKishiScriptInterface<INameMapTree> &Target, FName name)
{
    checkf(Target, TEXT("Error : Target is NULL or does not implement interface IArrayTree\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    auto result = INameMapTree::Execute_GetChildByName(Target.GetObject(), name);
    return result;
}
TArray<FName> UNameMapTreeLibrary::IGetNames(const TKishiScriptInterface<INameMapTree> &Target)
{
    checkf(Target, TEXT("Error : Target is NULL or does not implement interface IArrayTree\n GetObject: %d\n GetInterface: %d"), Target.GetObject(), Target.GetInterface());
    auto result = INameMapTree::Execute_GetNames(Target.GetObject());
    return result;
}

//Library

FName UNameMapTreeLibrary::GetName(const TScriptInterface<INameMapTree> &Target)
{
    auto parent = IGetParentTree(Target);
    auto names = IGetNames(parent);
    for (auto &&name : names)
    {
        TKishiScriptInterface<INameMapTree> child = IGetChildByName(parent, name);
        if (child == Target)
        {
            return name;
        }
    }
    checkNoEntry();
    return FName();
}

TArray<FName> UNameMapTreeLibrary::GetNamePath(const TScriptInterface<INameMapTree> &Target)
{
    if (IsRoot(Target.GetObject()))
    {
        return {};
    }
    return GetNamePath(IGetParentTree(Target).GetObject()) += {GetName(Target)};
}
bool UNameMapTreeLibrary::IsParentOfWithNamePath(const TScriptInterface<INameMapTree> &Target, const TScriptInterface<INameMapTree> &Child, TArray<FName> &Path)
{
    return IsChildOfWithNamePath(Child, Target, Path);
}

bool UNameMapTreeLibrary::IsChildOfWithNamePath(const TScriptInterface<INameMapTree> &Target, const TScriptInterface<INameMapTree> &Parent, TArray<FName> &Path)
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
    if (IsChildOfWithNamePath(IGetParentTree(Target.GetObject()).GetObject(), Parent, Path))
    {
        Path.Add(GetName(Target));
        return true;
    }
    return false;
}

TScriptInterface<INameMapTree> UNameMapTreeLibrary::GetChildAtNamePath(const TScriptInterface<INameMapTree> &Target, TArray<FName> Path)
{
    if (Path.Num() == 0)
        return Target;
    auto child = IGetChildByName(Target, Path[0]);
    Path.RemoveAt(0);
    if (child.GetObject())
        return GetChildAtNamePath(child, Path);
    return NULL;
}

bool UNameMapTreeLibrary::NameToIndex(const TScriptInterface<INameMapTree> &Target, FName name, uint8 &index)
{
    auto names = IGetNames(Target);
    for (uint8 i = 0; i < names.Num(); ++i)
    {
        if (name == names[i])
        {
            index = i;
            return true;
        }
    }
    return false;
}

bool UNameMapTreeLibrary::IndexToName(const TScriptInterface<INameMapTree> &Target, uint8 index, FName &name)
{
    auto names = IGetNames(Target);
    if (index < names.Num())
    {
        name = names[index];
        return true;
    }
    return false;
}

bool UNameMapTreeLibrary::NamePathToIndexPath(const TScriptInterface<INameMapTree> &Target, TArray<FName> namePath, TArray<uint8> &indexPath)
{
    if (namePath.Num() == 1)
    {
        uint8 index;
        if (NameToIndex(Target, namePath[0], index))
        {
            indexPath.Add(index);
            return true;
        }
        return false;
    }
    uint8 index;
    if (NameToIndex(Target, namePath[0], index))
    {
        auto Child = IGetChildByName(Target, namePath[0]);
        indexPath.Add(index);
        namePath.RemoveAt(0);
        return NamePathToIndexPath(Child, namePath, indexPath);
    }
    return false;
}

bool UNameMapTreeLibrary::IndexPathToNamePath(const TScriptInterface<INameMapTree> &Target, TArray<uint8> indexPath, TArray<FName> &namePath)
{
    if (indexPath.Num() == 1)
    {
        FName name;
        if (IndexToName(Target, indexPath[0], name))
        {
            namePath.Add(name);
            return true;
        }
        return false;
    }
    FName name;
    if (IndexToName(Target, indexPath[0], name))
    {
        auto Child = IGetChildByName(Target, name);
        namePath.Add(name);
        indexPath.RemoveAt(0);
        return IndexPathToNamePath(Child, indexPath, namePath);
    }
    return false;
}
