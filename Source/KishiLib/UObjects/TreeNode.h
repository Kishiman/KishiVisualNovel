// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/GeneratedCppIncludes.h"
#include "Templates/Tree.h"
#include "TreeNode.generated.h"
/**
 *
 */
UCLASS()
class KISHILIB_API UTreeNode : public UObject
{
    GENERATED_BODY()
    //friend UTreeNode;

public:
    TTree<UObject> rootTree;
    UTreeNode(TTree<UObject> &tree) : rootTree(tree) {}
    //UFUNCTION(BlueprintCallable)

    UTreeNode(UTreeNode &treeNode) : rootTree(treeNode.rootTree) {}
    //UFUNCTION(BlueprintCallable)
    UTreeNode(UObject &object) : rootTree(object) {}

    UTreeNode(UObject &object, TArray<UTreeNode *> children), rootTree(object)
    {
        for (int i = 0; i < children.Num(); i++)
        {
            rootTree.AddBranch(children[i]->rootTree);
        }
    }
    //UFunction(BlueprintPure)
    UTreeNode() = default;

    UFUNCTION(BlueprintCallable)
    static UTreeNode *Copy(const UTreeNode *treeNode)
    {
        return UTreeNode(*treeNode);
    }

    UFUNCTION(BlueprintCallable)
    static UTreeNode *CreateFromObject(UObject *object)
    {
        return UTreeNode(*object);
    }
    UFUNCTION(BlueprintCallable)
    static UTreeNode *CreateWithChildren(UObject *object, TArray<UTreeNode *> children)
    {
        return UTreeNode(*object, children);
    }
    //UFunction()

    ~UTreeNode() = default;
    UFUNCTION(BlueprintPure)
    bool IsRoot() { return rootTree.IsRoot(); }
    UFUNCTION(BlueprintPure)
    bool IsLeaf() { return rootTree.IsLeaf(); }
    UFUNCTION(BlueprintCallable)
    UTreeNode *AddBranch(UTreeNode *otherNode)
    {
        return new UTreeNode(this->rootTree.AddBranch(otherNode->rootTree));
    }

    UFUNCTION(BlueprintCallable)
    UTreeNode *EmplaceAt(int index, UTreeNode *otherNode)
    {
        return new UTreeNode(this->rootTree.EmplaceAt(index, otherNode->rootTree));
    }

    UFUNCTION(BlueprintCallable)
    UTreeNode *GetChild(TArray<int> &path)
    {
        return new UTreeNode(rootTree.GetChild(path));
    }

    UFUNCTION(BlueprintCallable)
    bool Find(UTreeNode *node, TArray<int> &path)
    {
        if (rootTree.Find(node->rootTree, [](TTree<UObject> tree1, TTree<UObject> tree2) {
                return tree1 == tree2;
            },
                          path))
        {
            SwapAll(path);
            return true;
        }
        return false;
    }

protected:
private:
};