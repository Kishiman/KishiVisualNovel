#pragma once

#include "CoreMinimal.h"
// #include "Kismet/BlueprintFunctionLibrary.h"
// #include "Templates/KishiScriptInterface.h"

template <typename T>
struct THTreeNode
{
public:
    virtual operator bool() = 0;
    virtual operator T() = 0;
    virtual THTreeNode<T> GetParent() const = 0;
    virtual TArray<THTreeNode<T>> GetDirectChildren() const = 0;
    virtual int GetDirectChildrenSize() const = 0;
    bool IsLeaf() const;
    bool IsRoot() const;
    bool IsDirectParentOf(THTreeNode<T> Child) const;
    bool IsDirectChildOf(THTreeNode<T> Parent) const;
    bool IsParentOf(THTreeNode<T> Child) const;
    bool IsParentOfWithDistance(THTreeNode<T> Child, int &Distance) const;
    bool IsParentOfWithNodes(THTreeNode<T> Child, TArray<THTreeNode<T>> &Nodes) const;
    // int GetDepth() const;
    // int GetMaxDepthToLeafs() const;
    // int GetMinDepthToLeafs() const;
    // T GetRootParent() const;
    // TArray<T> GetParentsFromRoot() const;
    // int GetSize() const;
    // int GetLeafsSize() const;
    // int GetTreeSizeToDepth(int depth);
    // int GetTreeSizeAtDepth(int depth);
    // TArray<T> GetTree() const;
    // TArray<T> GetLeafs() const;
    // TArray<T> GetDirectLeafs() const;
    // TArray<T> GetDirectNonLeafs() const;
    // TArray<T> GetChildrenAtDepth(int depth);
    // TArray<T> GetChildrenToDepth(int depth);
};