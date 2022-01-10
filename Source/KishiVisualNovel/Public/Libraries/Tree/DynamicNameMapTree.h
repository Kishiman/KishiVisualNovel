#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NameMapTree.h"

#include "DynamicNameMapTree.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API UDynamicNameMapTree : public UNameMapTree
{
    GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IDynamicNameMapTree : public INameMapTree
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Tree|Array|NameMap")
    TScriptInterface<IDynamicNameMapTree> CreateTree() const;
    virtual TScriptInterface<IDynamicNameMapTree> CreateTree_Implementation() const = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Tree|Dynamic")
    bool AddChildAt(FName key, const TScriptInterface<IDynamicNameMapTree> &ChildToAdd);
    virtual bool AddChildAt_Implementation(FName key, const TScriptInterface<IDynamicNameMapTree> &ChildToAdd) = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Tree|Dynamic")
    bool RemoveChild(const TScriptInterface<IDynamicNameMapTree> &ChildToRemove);
    virtual bool RemoveChild_Implementation(const TScriptInterface<IDynamicNameMapTree> &ChildToRemove) = 0;
};

UCLASS()
class KISHIVISUALNOVEL_API UDynamicNameMapTreeLibrary : public UNameMapTreeLibrary
{
    GENERATED_BODY()
public:
    /*
    Interface Proxy Functions
    */
    static TKishiScriptInterface<IDynamicNameMapTree> ICreateTree(const TKishiScriptInterface<IDynamicNameMapTree> &Target);
    static bool IAddChildAt(const TKishiScriptInterface<IDynamicNameMapTree> &Target, FName key, const TKishiScriptInterface<IDynamicNameMapTree> &ChildToAdd);
    static bool IRemoveChild(const TKishiScriptInterface<IDynamicNameMapTree> &Target, const TKishiScriptInterface<IDynamicNameMapTree> &ChildToRemove);

    // UFUNCTION(BlueprintCallable)
    // static bool AddNewChildAt(const TScriptInterface<IDynamicNameMapTree> &Target, FName key, TScriptInterface<IDynamicNameMapTree> &ChildAdded);
    // UFUNCTION(BlueprintCallable)
    // static bool AddNewChildToParentAt(const TScriptInterface<IDynamicNameMapTree> &Target, FName key, TScriptInterface<IDynamicNameMapTree> &TreeAdded, uint8 parentIndex = 1);

    // UFUNCTION(BlueprintCallable)
    // static bool AddNewChilden(const TScriptInterface<IDynamicNameMapTree> &Target, TArray<TScriptInterface<IDynamicNameMapTree>> &ChildrenAdded, uint8 count = 1);
    // UFUNCTION(BlueprintCallable)
    // static bool AddNewChildrenToParent(const TScriptInterface<IDynamicNameMapTree> &Target, TArray<TScriptInterface<IDynamicNameMapTree>> &TreesAdded, uint8 count, uint8 parentIndex = 1);

    // UFUNCTION(BlueprintCallable)
    // static bool JoinTwoTrees(const TScriptInterface<IDynamicNameMapTree> &Target, TScriptInterface<IDynamicNameMapTree> &AnotherTree, TScriptInterface<IDynamicNameMapTree> &NewParentTree);
    // UFUNCTION(BlueprintCallable)
    // static bool JoinTrees(const TArray<TScriptInterface<IDynamicNameMapTree>> &Trees, TScriptInterface<IDynamicNameMapTree> &NewParentTree);

    // UFUNCTION(BlueprintCallable)
    // static bool RemoveFromParent(const TScriptInterface<IDynamicNameMapTree> &Target);
    // UFUNCTION(BlueprintCallable)
    // static bool RemoveParentFromParent(const TScriptInterface<IDynamicNameMapTree> &Target, uint8 ParentIndex = 1);
    // UFUNCTION(BlueprintCallable)
    // static bool RemoveDirectChildren(const TScriptInterface<IDynamicNameMapTree> &Target, const TArray<TScriptInterface<IDynamicNameMapTree>> &DirectChildrenToRemove);
    // UFUNCTION(BlueprintCallable)
    // static bool RemoveChildren(const TScriptInterface<IDynamicNameMapTree> &Target, const TArray<TScriptInterface<IDynamicNameMapTree>> &ChildrenToRemove);
    // UFUNCTION(BlueprintCallable)
    // static bool RemoveAllDirectChildren(const TScriptInterface<IDynamicNameMapTree> &Target, TArray<TScriptInterface<IDynamicNameMapTree>> &DirectChildrenRemoved);
    // UFUNCTION(BlueprintCallable)
    // static bool RemoveAllChildren(const TScriptInterface<IDynamicNameMapTree> &Target, TArray<TScriptInterface<IDynamicNameMapTree>> &ChildrenRemoved);

    // UFUNCTION(BlueprintCallable)
    // static bool RemoveDirectLeafs(const TScriptInterface<IDynamicNameMapTree> &Target, TArray<TScriptInterface<IDynamicNameMapTree>> &DirectLeafsRemoved);
    // UFUNCTION(BlueprintCallable)
    // static bool RemoveDirectNonLeafs(const TScriptInterface<IDynamicNameMapTree> &Target, TArray<TScriptInterface<IDynamicNameMapTree>> &DirectNonLeafsRemoved);
    // UFUNCTION(BlueprintCallable)
    // static bool MakeAllDirectChildrenLeafs(const TScriptInterface<IDynamicNameMapTree> &Target, TArray<TScriptInterface<IDynamicNameMapTree>> &ChildrenRemoved);

    //Libray

protected:
private:
};