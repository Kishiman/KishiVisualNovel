#include "Theories/TreeNode.h"
// #include "Templates/KishiScriptInterface.h"
// #include "Macros/Interface.h"
#include "Algo/Reverse.h"

template <typename T>
bool THTreeNode<T>::IsLeaf() const
{
    auto childrenSize = this->GetDirectChildrenSize();
    return childrenSize == 0;
}
template <typename T>
bool THTreeNode<T>::IsRoot() const
{
    auto parent = this->GetParent();
    return parent.GetObject() == NULL;
}
template <typename T>
bool THTreeNode<T>::IsDirectParentOf(THTreeNode<T> Child) const
{
    auto parent = Child.GetParent();
    return parent == *this;
}
template <typename T>
bool THTreeNode<T>::IsDirectChildOf(THTreeNode<T> Parent) const
{
    return Parent->IsDirectParentOf(*this);
}
template <typename T>
bool THTreeNode<T>::IsParentOf(THTreeNode<T> Child) const
{
    TArray<THTreeNode<T>> Nodes;
    return this->IsParentOf(Child, Nodes);
}
template <typename T>
bool THTreeNode<T>::IsParentOfWithDistance(THTreeNode<T> Child, int &Distance) const
{
    TArray<THTreeNode<T>> Nodes;
    bool result = this->IsParentOf(Child, Nodes);
    Distance = Nodes.Num();
    return result;
}
template <typename T>
bool THTreeNode<T>::IsParentOfWithNodes(THTreeNode<T> Child, TArray<THTreeNode<T>> &Nodes) const
{
    Nodes.Empty();
    auto parent = Child;
    while (parent && parent != *this)
    {
        Nodes.Add(parent);
        parent = parent->GetParentTree();
    }
    if (!parent)
        return false;
    Algo::Reverse(Nodes);
    return true;
}

// template <typename T>
// int THTreeNode<T>::GetDepth() const
// {
//     if (this->IsRoot())
//         return 0;
//     auto parent = this->GetParent();
//     return parent.GetDepth() + 1;
// }
// template <typename T>
// int THTreeNode<T>::GetMaxDepthToLeafs() const
// {
//     int max = 0;
//     auto children = this->GetDirectChildren();
//     for (auto child : children)
//     {
//         max = FMath::Max<int>(max,child.GetMaxDepthToLeafs() + 1);
//     }
//     return max;
// }
// template <typename T>
// int THTreeNode<T>::GetMinDepthToLeafs() const
// {
//     int min = 0;
//     auto children = this->GetDirectChildren();
//     for (auto child : children)
//     {
//         min = FMath::Min<int>(min, child.GetMinDepthToLeafs() + 1);
//     }
//     return min;
// }
// template <typename T>
// T THTreeNode<T>::GetRootParent() const
// {
//     if (this->IsRoot())
//         return this;
//     return this->GetParent().GetRootParent();
// }
// template <typename T>
// TArray<THTreeNode<T>> THTreeNode<T>::GetParentsFromRoot() const
// {
//     if (this->IsRoot())
//         return {};
//     return GetParentsFromRoot(this->GetParent().GetObject()) += {this};
// }
// template <typename T>
// int THTreeNode<T>::GetSize() const
// {
//     int size = 1;
//     auto children = this->GetDirectChildren();
//     for (auto child : children)
//     {
//         size += GetSize(child);
//     }
//     return size;
// }
// template <typename T>
// int THTreeNode<T>::GetLeafsSize() const
// {
//     if (IsLeaf(*this))
//         return 1;
//     int size = 0;
//     auto children = this->GetDirectChildren();
//     for (auto child : children)
//     {
//         size += GetLeafsSize(child);
//     }
//     return size;
// }
// template <typename T>
// int THTreeNode<T>::GetTreeSizeToDepth(int depth) const
// {

//     if (depth < 0)
//     {
//         UE_LOG(LogTemp, Warning, TEXT("depth = %d <0!"), depth);
//     }
//     if (depth == 0)
//         return 1;
//     if (depth == 1)
//         return this->GetDirectChildrenSize();
//     int size = 1;
//     auto children = this->GetDirectChildren();
//     for (auto child : children)
//     {
//         size += GetTreeSizeToDepth(child, depth - 1);
//     }
//     return size;
// }
// template <typename T>
// int THTreeNode<T>::GetTreeSizeAtDepth(int depth) const
// {

//     if (depth < 0)
//     {
//         UE_LOG(LogTemp, Warning, TEXT("depth = %d <0!"), depth);
//     }
//     if (depth == 0)
//         return 1;
//     if (depth == 1)
//         return this->GetDirectChildrenSize();
//     int size = 0;
//     auto children = this->GetDirectChildren();
//     for (auto child : children)
//     {
//         size += GetTreeSizeAtDepth(child, depth - 1);
//     }
//     return size;
// }
// template <typename T>
// TArray<THTreeNode<T>> THTreeNode<T>::GetTree() const
// {
//     TArray<THTreeNode<T>> out({this});
//     auto children = this->GetDirectChildren();
//     for (auto child : children)
//     {
//         out.Append(GetTree(child));
//     }
//     return out;
// }
// template <typename T>
// TArray<THTreeNode<T>> THTreeNode<T>::GetLeafs() const
// {
//     if (IsLeaf(*this))
//         return {this};
//     TArray<THTreeNode<T>> out;
//     auto children = this->GetDirectChildren();
//     for (auto child : children)
//     {
//         out.Append(GetLeafs(child));
//     }
//     return out;
// }
// template <typename T>
// TArray<THTreeNode<T>> THTreeNode<T>::GetDirectLeafs() const
// {
//     if (IsLeaf(*this))
//         return {};
//     auto children = this->GetDirectChildren();
//     TArray<THTreeNode<T>> out;
//     out.Reserve(children.Num());
//     for (auto child : children)
//     {
//         if (IsLeaf(child))
//             out.Add(child);
//     }
//     return out;
// }
// template <typename T>
// TArray<THTreeNode<T>> THTreeNode<T>::GetDirectNonLeafs() const
// {
//     if (IsLeaf(*this))
//         return {};
//     auto children = this->GetDirectChildren();
//     TArray<THTreeNode<T>> out;
//     out.Reserve(children.Num());
//     for (auto child : children)
//     {
//         if (!IsLeaf(child))
//             out.Add(child);
//     }
//     return out;
// }
// template <typename T>
// TArray<THTreeNode<T>> THTreeNode<T>::GetChildrenToDepth(int depth) const
// {

//     if (depth < 0)
//     {
//         UE_LOG(LogTemp, Warning, TEXT("depth = %d <0!"), depth);
//     }
//     if (depth == 0)
//         return {this};
//     if (depth == 1)
//         return this->GetDirectChildren();
//     TArray<THTreeNode<T>> out({this});
//     auto children = this->GetDirectChildren();
//     for (auto child : children)
//     {
//         out.Append(GetChildrenToDepth(child, depth - 1));
//     }
//     return out;
// }
// template <typename T>
// TArray<THTreeNode<T>> THTreeNode<T>::GetChildrenAtDepth(int depth) const
// {
//     if (depth < 0)
//     {
//         UE_LOG(LogTemp, Warning, TEXT("depth = %d <0!"), depth);
//     }
//     if (depth == 0)
//         return {this};
//     if (depth == 1)
//         return this->GetDirectChildren();
//     TArray<THTreeNode<T>> out;
//     auto children = this->GetDirectChildren();
//     for (auto child : children)
//     {
//         out.Append(GetChildrenAtDepth(child, depth - 1));
//     }
//     return out;
// }
