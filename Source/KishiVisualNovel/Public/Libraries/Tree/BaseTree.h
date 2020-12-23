#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/KishiScriptInterface.h"

#include "Libraries/Graph/BaseGraph.h"
#include "BaseTree.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API UBaseTree : public UInterface
{
    GENERATED_BODY()
};
class KISHIVISUALNOVEL_API IBaseTree
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    TScriptInterface<IBaseTree> GetParentTree() const;
    virtual TScriptInterface<IBaseTree> GetParentTree_Implementation() const = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    TArray<TScriptInterface<IBaseTree>> GetDirectChildren() const;
    virtual TArray<TScriptInterface<IBaseTree>> GetDirectChildren_Implementation() const = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    uint8 GetDirectChildrenSize() const;
    virtual uint8 GetDirectChildrenSize_Implementation() const = 0;
};

UCLASS()
class KISHIVISUALNOVEL_API UBaseTreeLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    /*
    Interface Proxy Functions
    */
    static TKishiScriptInterface<IBaseTree> IGetParentTree(TKishiScriptInterface<IBaseTree> Target);
    static TArray<TScriptInterface<IBaseTree>> IGetDirectChildren(TKishiScriptInterface<IBaseTree> Target);
    static uint8 IGetDirectChildrenSize(TKishiScriptInterface<IBaseTree> Target);
    /*
    DefaultImplementation
    */
    UFUNCTION(BlueprintPure)
    static uint8 IGetDirectChildrenSize_Default(const TScriptInterface<IBaseTree> &Target);

    /*
    Library
    */
    UFUNCTION(BlueprintPure)
    static bool IsLeaf(const TScriptInterface<IBaseTree> &Target);
    UFUNCTION(BlueprintPure)
    static bool IsRoot(const TScriptInterface<IBaseTree> &Target);

    UFUNCTION(BlueprintPure)
    static bool IsDirectParentOf(const TScriptInterface<IBaseTree> &Target, const TScriptInterface<IBaseTree> &Child);
    UFUNCTION(BlueprintPure)
    static bool IsDirectChildOf(const TScriptInterface<IBaseTree> &Target, const TScriptInterface<IBaseTree> &Parent);
    UFUNCTION(BlueprintPure)
    static bool IsParentOf(const TScriptInterface<IBaseTree> &Target, const TScriptInterface<IBaseTree> &Child);
    UFUNCTION(BlueprintPure)
    static bool IsChildOf(const TScriptInterface<IBaseTree> &Target, const TScriptInterface<IBaseTree> &Parent);
    UFUNCTION(BlueprintPure)
    static bool IsParentOfWithNodes(const TScriptInterface<IBaseTree> &Target, const TScriptInterface<IBaseTree> &Child, TArray<TScriptInterface<IBaseTree>> &Nodes);
    UFUNCTION(BlueprintPure)
    static bool IsChildOfWithNodes(const TScriptInterface<IBaseTree> &Target, const TScriptInterface<IBaseTree> &Parent, TArray<TScriptInterface<IBaseTree>> &Nodes);
    UFUNCTION(BlueprintPure)
    static bool IsParentOfWithDistance(const TScriptInterface<IBaseTree> &Target, const TScriptInterface<IBaseTree> &Child, uint8 &Distance);
    UFUNCTION(BlueprintPure)
    static bool IsChildOfWithDistance(const TScriptInterface<IBaseTree> &Target, const TScriptInterface<IBaseTree> &Parent, uint8 &Distance);

    UFUNCTION(BlueprintPure)
    static uint8 GetDepth(const TScriptInterface<IBaseTree> &Target);
    UFUNCTION(BlueprintPure)
    static uint8 GetMaxDepthToLeafs(const TScriptInterface<IBaseTree> &Target);
    UFUNCTION(BlueprintPure)
    static uint8 GetMinDepthToLeafs(const TScriptInterface<IBaseTree> &Target);

    UFUNCTION(BlueprintPure)
    static TScriptInterface<IBaseTree> GetRootParent(const TScriptInterface<IBaseTree> &Target);
    UFUNCTION(BlueprintPure)
    static TArray<TScriptInterface<IBaseTree>> GetParentsFromRoot(const TScriptInterface<IBaseTree> &Target);

    UFUNCTION(BlueprintPure)
    static uint8 GetSize(const TScriptInterface<IBaseTree> &Target);
    UFUNCTION(BlueprintPure)
    static uint8 GetLeafsSize(const TScriptInterface<IBaseTree> &Target);
    UFUNCTION(BlueprintPure)
    static uint8 GetTreeSizeToDepth(const TScriptInterface<IBaseTree> &Target, uint8 depth);
    UFUNCTION(BlueprintPure)
    static uint8 GetTreeSizeAtDepth(const TScriptInterface<IBaseTree> &Target, uint8 depth);

    UFUNCTION(BlueprintPure)
    static TArray<TScriptInterface<IBaseTree>> GetTree(const TScriptInterface<IBaseTree> &Target);
    UFUNCTION(BlueprintPure)
    static TArray<TScriptInterface<IBaseTree>> GetLeafs(const TScriptInterface<IBaseTree> &Target);
    UFUNCTION(BlueprintPure)
    static TArray<TScriptInterface<IBaseTree>> GetDirectLeafs(const TScriptInterface<IBaseTree> &Target);
    UFUNCTION(BlueprintPure)
    static TArray<TScriptInterface<IBaseTree>> GetDirectNonLeafs(const TScriptInterface<IBaseTree> &Target);

    UFUNCTION(BlueprintPure)
    static TArray<TScriptInterface<IBaseTree>> GetChildrenAtDepth(const TScriptInterface<IBaseTree> &Target, uint8 depth);
    UFUNCTION(BlueprintPure)
    static TArray<TScriptInterface<IBaseTree>> GetChildrenToDepth(const TScriptInterface<IBaseTree> &Target, uint8 depth);

protected:
private:
};