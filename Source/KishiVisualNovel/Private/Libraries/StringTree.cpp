#include "Interfaces/StringTree.h"
#include "Misc/Paths.h"

bool UStringTreeLibrary::IsLeaf(const TScriptInterface<IStringTree> &Target)
{
    auto children = ISTRINGTREE(Target, GetChildren);
    return children.Num() == 0; //TODO;
}

bool UStringTreeLibrary::IsRoot(const TScriptInterface<IStringTree> &Target)
{
    auto parent = ISTRINGTREE(Target, GetParentTree);
    return !parent; //TODO;
}

bool UStringTreeLibrary::IsDirectParentOf(const TScriptInterface<IStringTree> &Target, const TScriptInterface<IStringTree> &Child)
{
    auto parent = ISTRINGTREE(Target, GetParentTree);
    return !parent && parent == Target; //TODO;
}

bool UStringTreeLibrary::IsDirectChildOf(const TScriptInterface<IStringTree> &Target, const TScriptInterface<IStringTree> &Parent)
{
    return IsDirectParentOf(Parent, Target); //TODO;
}

bool UStringTreeLibrary::AddChildAtName(const TScriptInterface<IStringTree> &Target, const TScriptInterface<IStringTree> &Child, const FString &Name)
{
    ISTRINGTREE_1(Child, SetName, Name);
    return ISTRINGTREE_1(Target, AddChild, Child);
}

//Return Child by name if exist, else Add Child

TScriptInterface<IStringTree> UStringTreeLibrary::ForceChild(const TScriptInterface<IStringTree> &Target, const FString &Name)
{
    auto child = ISTRINGTREE_1(Target, GetChild, Name);
    if (child.GetObject())
    {
        return child;
    }
    return ISTRINGTREE_1(Target, AddNewChild, Name);
}

bool UStringTreeLibrary::RemoveChild(const TScriptInterface<IStringTree> &Target, const FString &Name)
{
    auto child = ISTRINGTREE_1(Target, GetChild, Name);
    if (child.GetObject())
    {
        ISTRINGTREE_1(Target, RemoveChild, child);
        return true;
    };
    return false;
}

//SubChildrenLibrary

FString UStringTreeLibrary::GetPath(const TScriptInterface<IStringTree> &Target)
{
    auto parents = GetParentsFromRoot(Target);
    TArray<FString> names;
    for (auto parent : parents)
    {
        names.Add(ISTRINGTREE(parent, GetName));
    }
    return FString::Join(names, TEXT("/"));
}

TScriptInterface<IStringTree> UStringTreeLibrary::GetRootParent(const TScriptInterface<IStringTree> &Target)
{
    if (IsRoot(Target))
        return Target;
    return ISTRINGTREE(Target, GetParentTree);
}

TArray<TScriptInterface<IStringTree>> UStringTreeLibrary::GetParentsFromRoot(const TScriptInterface<IStringTree> &Target)
{
    if (IsRoot(Target))
        return TArray<TScriptInterface<IStringTree>>({Target});
    auto parent = ISTRINGTREE(Target, GetParentTree);
    auto parents = GetParentsFromRoot(parent);
    parents.Add(Target);
    return parents;
}

TScriptInterface<IStringTree> UStringTreeLibrary::GetChildAt(const TScriptInterface<IStringTree> &Target, FString Path)
{
    FString childname, pathFromChild;
    if (Path.Split(FString("/"), &childname, &pathFromChild))
    {
        auto child = ISTRINGTREE_1(Target, GetChild, childname);
        if (child.GetObject())
        {
            return (GetChildAt(child, pathFromChild));
        }
        return false;
    }
    return ISTRINGTREE_1(Target, GetChild, Path);
}

bool UStringTreeLibrary::IsParentOf(const TScriptInterface<IStringTree> &Target, const TScriptInterface<IStringTree> &Child, FString &OutPathP2C)
{

    FString StaticPath = OutPathP2C + "/" + ISTRINGTREE(Target, GetName);
    FString DynamicPath = StaticPath;
    if (Target.GetObject() == Child.GetObject())
    {
        OutPathP2C = StaticPath;
        return true;
    }
    auto children = ISTRINGTREE(Target, GetChildren);
    for (auto child : children)
    {
        if (IsParentOf(child, Child, DynamicPath))
        {
            OutPathP2C = DynamicPath;
            return true;
        }
        DynamicPath = StaticPath;
    }
    OutPathP2C = "";
    return false;
}

bool UStringTreeLibrary::IsChildOf(const TScriptInterface<IStringTree> &Target, const TScriptInterface<IStringTree> &Parent, FString &OutPathP2C)
{
    return IsParentOf(Parent, Target, OutPathP2C);
}

TArray<TScriptInterface<IStringTree>> UStringTreeLibrary::GetChildrenUnder(const TScriptInterface<IStringTree> &Target, FString Path)
{
    auto child = GetChildAt(Target, Path);
    if (child.GetObject())
    {
        return ISTRINGTREE(child, GetChildren);
    }
    return TArray<TScriptInterface<IStringTree>>();
}

TScriptInterface<IStringTree> UStringTreeLibrary::FindAnyChildByName(const TScriptInterface<IStringTree> &Target, const FString &Name, FString &OutPathP2C)
{
    FString StaticPath = OutPathP2C + "/" + ISTRINGTREE(Target, GetName);
    FString DynamicPath = StaticPath;
    if (ISTRINGTREE(Target, GetName) == Name)
    {
        OutPathP2C = StaticPath;
        return Target;
    }
    auto children = ISTRINGTREE(Target, GetChildren);
    for (auto child : children)
    {
        auto result = FindAnyChildByName(child, Name, DynamicPath);
        if (result.GetObject())
        {
            OutPathP2C = DynamicPath;
            return result;
        }
        DynamicPath = StaticPath;
    }
    OutPathP2C = "";
    return NULL;
}

bool UStringTreeLibrary::AddChildAt(const TScriptInterface<IStringTree> &Target, FString Path, const TScriptInterface<IStringTree> &InChild)
{
    FString childname, pathFromChild;
    if (Path.Split(FString("/"), &childname, &pathFromChild))
    {
        auto child = ForceChild(Target, childname);
        if (child.GetObject())
        {
            return AddChildAt(child, pathFromChild, InChild);
        }
        return false;
    }
    return AddChildAtName(Target, InChild, Path);
}
bool UStringTreeLibrary::AddChildUnder(const TScriptInterface<IStringTree> &Target, FString Path, const TScriptInterface<IStringTree> &InChild)
{
    return AddChildAt(Target, Path + "/" + ISTRINGTREE(InChild, GetName), InChild);
}

TScriptInterface<IStringTree> UStringTreeLibrary::AddNewChildAt(const TScriptInterface<IStringTree> &Target, FString Path)
{
    FString childname, pathFromChild;
    if (Path.Split(FString("/"), &childname, &pathFromChild))
    {
        auto child = ForceChild(Target, childname);
        if (child.GetObject())
        {
            return AddNewChildAt(child, pathFromChild);
        }
        UE_LOG(LogTemp, Error, TEXT("ForceChild Returned NULL"));
        return NULL;
    }
    return ISTRINGTREE_1(Target, AddNewChild, Path);
}

TScriptInterface<IStringTree> UStringTreeLibrary::AddNewChildUnder(const TScriptInterface<IStringTree> &Target, FString Path, const FString &Name)
{
    return AddNewChildAt(Target, Path + "/" + Name);
}

TScriptInterface<IStringTree> UStringTreeLibrary::ForceChildAt(const TScriptInterface<IStringTree> &Target, FString Path)
{
    FString childname, pathFromChild;
    if (Path.Split(FString("/"), &childname, &pathFromChild))
    {
        auto child = ForceChild(Target, childname);
        if (child.GetObject())
        {
            return ForceChildAt(child, pathFromChild);
        }
        UE_LOG(LogTemp, Error, TEXT("ForceChild Returned NULL"));
        return NULL;
    }
    return ForceChild(Target, Path);
}

TScriptInterface<IStringTree> UStringTreeLibrary::ForceChildUnder(const TScriptInterface<IStringTree> &Target, FString Path, const FString &Name)
{
    return ForceChildAt(Target, Path + "/" + Name);
}

bool UStringTreeLibrary::RemoveChildAt(const TScriptInterface<IStringTree> &Target, FString Path)
{
    FString childname, pathFromChild;
    if (Path.Split(FString("/"), &childname, &pathFromChild))
    {
        auto child = ISTRINGTREE_1(Target, GetChild, childname);
        if (child.GetObject())
        {
            return (RemoveChildAt(child, pathFromChild));
        }
        return false;
    }
    return RemoveChild(Target, Path);
}