#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BaseTree.h"

#include "DynamicTree.generated.h"

UINTERFACE(BlueprintType)
class KISHI_API UDynamicTree : public UBaseTree
{
    GENERATED_BODY()
};

class KISHI_API IDynamicTree : public IBaseTree
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Tree|Dynamic")
    TScriptInterface<IDynamicTree> CreateTree();
    virtual TScriptInterface<IDynamicTree> CreateTree_Implementation() = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Tree|Dynamic")
    bool AddChild(const TScriptInterface<IDynamicTree> &ChildToAdd);
    virtual bool AddChild_Implementation(const TScriptInterface<IDynamicTree> &ChildToAdd) = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Tree|Dynamic")
    bool RemoveChild(const TScriptInterface<IDynamicTree> &ChildToRemove);
    virtual bool RemoveChild_Implementation(const TScriptInterface<IDynamicTree> &ChildToRemove) = 0;
};

UCLASS()
class KISHI_API UDynamicTreeLibrary : public UBaseTreeLibrary
{
    GENERATED_BODY()
public:
    /*
    Interface Proxy Functions
    */
    static TKishiScriptInterface<IDynamicTree> ICreateTree(const TKishiScriptInterface<IDynamicTree> &Target);
    static bool IAddChild(const TKishiScriptInterface<IDynamicTree> &Target, const TKishiScriptInterface<IDynamicTree> &ChildToAdd);
    static bool IRemoveChild(const TKishiScriptInterface<IDynamicTree> &Target, const TKishiScriptInterface<IDynamicTree> &ChildToRemove);

    UFUNCTION(BlueprintCallable)
    static bool AddNewChild(const TScriptInterface<IDynamicTree> &Target, TScriptInterface<IDynamicTree> &ChildAdded);
    UFUNCTION(BlueprintCallable)
    static bool AddNewChildToParent(const TScriptInterface<IDynamicTree> &Target, TScriptInterface<IDynamicTree> &TreeAdded, uint8 parentIndex = 1);

    UFUNCTION(BlueprintCallable)
    static bool AddNewChilden(const TScriptInterface<IDynamicTree> &Target, TArray<TScriptInterface<IDynamicTree>> &ChildrenAdded, uint8 count = 1);
    UFUNCTION(BlueprintCallable)
    static bool AddNewChildrenToParent(const TScriptInterface<IDynamicTree> &Target, TArray<TScriptInterface<IDynamicTree>> &TreesAdded, uint8 count, uint8 parentIndex = 1);

    UFUNCTION(BlueprintCallable)
    static bool JoinTwoTrees(const TScriptInterface<IDynamicTree> &Target, TScriptInterface<IDynamicTree> &AnotherTree, TScriptInterface<IDynamicTree> &NewParentTree);
    UFUNCTION(BlueprintCallable)
    static bool JoinTrees(const TArray<TScriptInterface<IDynamicTree>> &Trees, TScriptInterface<IDynamicTree> &NewParentTree);

    UFUNCTION(BlueprintCallable)
    static bool RemoveFromParent(const TScriptInterface<IDynamicTree> &Target);
    UFUNCTION(BlueprintCallable)
    static bool RemoveParentFromParent(const TScriptInterface<IDynamicTree> &Target, uint8 ParentIndex = 1);
    UFUNCTION(BlueprintCallable)
    static bool RemoveDirectChildren(const TScriptInterface<IDynamicTree> &Target, const TArray<TScriptInterface<IDynamicTree>> &DirectChildrenToRemove);
    UFUNCTION(BlueprintCallable)
    static bool RemoveChildren(const TScriptInterface<IDynamicTree> &Target, const TArray<TScriptInterface<IDynamicTree>> &ChildrenToRemove);
    UFUNCTION(BlueprintCallable)
    static bool RemoveAllDirectChildren(const TScriptInterface<IDynamicTree> &Target, TArray<TScriptInterface<IDynamicTree>> &DirectChildrenRemoved);
    UFUNCTION(BlueprintCallable)
    static bool RemoveAllChildren(const TScriptInterface<IDynamicTree> &Target, TArray<TScriptInterface<IDynamicTree>> &ChildrenRemoved);

    UFUNCTION(BlueprintCallable)
    static bool RemoveDirectLeafs(const TScriptInterface<IDynamicTree> &Target, TArray<TScriptInterface<IDynamicTree>> &DirectLeafsRemoved);
    UFUNCTION(BlueprintCallable)
    static bool RemoveDirectNonLeafs(const TScriptInterface<IDynamicTree> &Target, TArray<TScriptInterface<IDynamicTree>> &DirectNonLeafsRemoved);
    UFUNCTION(BlueprintCallable)
    static bool MakeAllDirectChildrenLeafs(const TScriptInterface<IDynamicTree> &Target, TArray<TScriptInterface<IDynamicTree>> &ChildrenRemoved);

    // UFUNCTION(BlueprintCallable)
    // static bool RemoveAllLeafs(const TScriptInterface<IDynamicTree> &Target,TArray<TScriptInterface<IDynamicTree>>& LeafsRemoved);
    // UFUNCTION(BlueprintCallable)
    // static bool RemoveAllLeafsAtDepth(const TScriptInterface<IDynamicTree> &Target,TArray<TScriptInterface<IDynamicTree>>& LeafsRemoved,uint8 depth=1);
    // UFUNCTION(BlueprintCallable)
    // static bool RemoveAllLeafsAtDepth(const TScriptInterface<IDynamicTree> &Target,TArray<TScriptInterface<IDynamicTree>>& LeafsRemoved,uint8 depth=1);

protected:
private:
};