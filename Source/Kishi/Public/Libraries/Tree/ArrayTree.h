#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BaseTree.h"

#include "ArrayTree.generated.h"

UINTERFACE(BlueprintType)
class KISHI_API UArrayTree : public UBaseTree
{
    GENERATED_BODY()
};

class KISHI_API IArrayTree : public IBaseTree
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Tree|Array")
    TScriptInterface<IArrayTree> GetChild(uint8 index) const;
    virtual TScriptInterface<IArrayTree> GetChild_Implementation(uint8 index) const = 0;
    //BaseTree overrides
    virtual TArray<TScriptInterface<IBaseTree>> GetDirectChildren_Implementation() const override;

};

UCLASS()
class KISHI_API UArrayTreeImplementation : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    /*
    Default Implementation
    */

    /*
    Overrides Implementation
    */
    UFUNCTION(BlueprintPure, meta = (DefaultToSelf = "Target"), Category = "Default|Tree|Array")
    static TArray<TScriptInterface<IBaseTree>> IGetDirectChildren_Default(const TScriptInterface<IArrayTree> &Target);

};

UCLASS(MinimalAPI)
class  UArrayTreeLibrary : public UBaseTreeLibrary
{
    GENERATED_BODY()
public:
    /*
    Interface Proxy Functions
    */
    static TKishiScriptInterface<IArrayTree> IGetChild(TKishiScriptInterface<IArrayTree> Target, uint8 index);


    UFUNCTION(BlueprintPure)
    static bool IsFirst(const TScriptInterface<IArrayTree> &Target);
    UFUNCTION(BlueprintPure)
    static bool IsLast(const TScriptInterface<IArrayTree> &Target);

    UFUNCTION(BlueprintPure)
    static uint8 GetIndex(const TScriptInterface<IArrayTree> &Target);
    UFUNCTION(BlueprintPure)
    static uint8 GetLeafIndex(const TScriptInterface<IArrayTree> &Target);
    UFUNCTION(BlueprintPure)
    static uint8 GetIndexInRoot(const TScriptInterface<IArrayTree> &Target);
    UFUNCTION(BlueprintPure)
    static uint8 GetIndexInParent(const TScriptInterface<IArrayTree> &Target, uint8 depth = 1);

    UFUNCTION(BlueprintPure)
    static TScriptInterface<IArrayTree> GetNotFirstParent(const TScriptInterface<IArrayTree> &Target, uint8 &OutDistance);
    UFUNCTION(BlueprintPure)
    static TScriptInterface<IArrayTree> GetNotLastParent(const TScriptInterface<IArrayTree> &Target, uint8 &OutDistance);

    UFUNCTION(BlueprintPure)
    static TScriptInterface<IArrayTree> GetFirstChild(const TScriptInterface<IArrayTree> &Target, bool bReturnTargetIfLeaf = true);
    UFUNCTION(BlueprintPure)
    static TScriptInterface<IArrayTree> GetLastChild(const TScriptInterface<IArrayTree> &Target, bool bReturnTargetIfLeaf = true);

    UFUNCTION(BlueprintPure)
    static TScriptInterface<IArrayTree> GetNext(const TScriptInterface<IArrayTree> &Target);
    UFUNCTION(BlueprintPure)
    static TScriptInterface<IArrayTree> GetPrevious(const TScriptInterface<IArrayTree> &Target);

    UFUNCTION(BlueprintPure)
    static TScriptInterface<IArrayTree> GetNextDirectSibling(const TScriptInterface<IArrayTree> &Target);
    UFUNCTION(BlueprintPure)
    static TScriptInterface<IArrayTree> GetPreviousDirectSibling(const TScriptInterface<IArrayTree> &Target);

    UFUNCTION(BlueprintPure)
    static TScriptInterface<IArrayTree> GetFirstLeaf(const TScriptInterface<IArrayTree> &Target);
    UFUNCTION(BlueprintPure)
    static TScriptInterface<IArrayTree> GetLastLeaf(const TScriptInterface<IArrayTree> &Target);
    UFUNCTION(BlueprintPure)
    static TScriptInterface<IArrayTree> GetNextLeaf(const TScriptInterface<IArrayTree> &Target);
    UFUNCTION(BlueprintPure)
    static TScriptInterface<IArrayTree> GetPreviousLeaf(const TScriptInterface<IArrayTree> &Target);

    UFUNCTION(BlueprintPure)
    static TScriptInterface<IArrayTree> GetFirstAtDepth(const TScriptInterface<IArrayTree> &Target, uint8 Depth);
    UFUNCTION(BlueprintPure)
    static TScriptInterface<IArrayTree> GetLastAtDepth(const TScriptInterface<IArrayTree> &Target, uint8 Depth);
    UFUNCTION(BlueprintPure)
    static TScriptInterface<IArrayTree> GetNextAtSameDepth(const TScriptInterface<IArrayTree> &Target);
    UFUNCTION(BlueprintPure)
    static TScriptInterface<IArrayTree> GetPreviousAtSameDepth(const TScriptInterface<IArrayTree> &Target);

    UFUNCTION(BlueprintPure)
    static TArray<uint8> GetPath(const TScriptInterface<IArrayTree> &Target);

    UFUNCTION(BlueprintPure)
    static bool IsDirectParentOfWithIndex(const TScriptInterface<IArrayTree> &Target, const TScriptInterface<IArrayTree> &Child, uint8 &IndexOfChild);
    UFUNCTION(BlueprintPure)
    static bool IsDirectChildOfWithIndex(const TScriptInterface<IArrayTree> &Target, const TScriptInterface<IArrayTree> &Parent, uint8 &IndexOfChild);

    UFUNCTION(BlueprintPure)
    static bool IsParentOfWithPath(const TScriptInterface<IArrayTree> &Target, const TScriptInterface<IArrayTree> &Child, TArray<uint8> &Path);
    UFUNCTION(BlueprintPure)
    static bool IsChildOfWithPath(const TScriptInterface<IArrayTree> &Target, const TScriptInterface<IArrayTree> &Parent, TArray<uint8> &Path);

    UFUNCTION(BlueprintPure)
    static TScriptInterface<IArrayTree> GetChildAtPath(const TScriptInterface<IArrayTree> &Target, TArray<uint8> Path);
    UFUNCTION(BlueprintPure)
    static TScriptInterface<IArrayTree> GetChildAtIndex(const TScriptInterface<IArrayTree> &Target, uint8 Index);
    UFUNCTION(BlueprintPure)
    static TScriptInterface<IArrayTree> GetChildAtRootIndex(const TScriptInterface<IArrayTree> &Target, uint8 Index);

protected:
private:
};