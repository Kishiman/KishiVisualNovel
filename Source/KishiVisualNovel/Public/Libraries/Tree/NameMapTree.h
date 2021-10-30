#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ArrayTree.h"

#include "NameMapTree.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API UNameMapTree : public UArrayTree
{
    GENERATED_BODY()
};

class KISHIVISUALNOVEL_API INameMapTree : public IArrayTree
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Tree|Array|NameMap")
    TScriptInterface<INameMapTree> GetChildByName(FName name) const;
    virtual TScriptInterface<INameMapTree> GetChildByName_Implementation(FName name) const = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Tree|Array|NameMap")
    TArray<FName> GetNames() const;
    virtual TArray<FName> GetNames_Implementation() const = 0;
    //IArrayTree overrides
    virtual TScriptInterface<IArrayTree> GetChild_Implementation(uint8 index) const override;
    //IArrayTree overrides
    virtual TArray<TScriptInterface<IBaseTree>> GetDirectChildren_Implementation() const override;
    virtual uint8 GetDirectChildrenSize_Implementation() const override;


};

UCLASS(MinimalAPI)
class  UNameMapTreeImplementation : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    /*
    Default Implementation
    */
    /*
    Overrides Implementation
    */
    UFUNCTION(BlueprintPure, meta = (DefaultToSelf = "Target"), Category = "Default|Tree|Array|NameMap")
    static TScriptInterface<IArrayTree> IGetChild_Default(const TScriptInterface<INameMapTree> &Target,uint8 index);
    UFUNCTION(BlueprintPure, meta = (DefaultToSelf = "Target"), Category = "Default|Tree|Array|NameMap")
    static TArray<TScriptInterface<IBaseTree>> IGetDirectChildren_Default(const TScriptInterface<INameMapTree> &Target);
    UFUNCTION(BlueprintPure, meta = (DefaultToSelf = "Target"), Category = "Default|Tree|Array|NameMap")
    static uint8 IGetDirectChildrenSize_Default(const TScriptInterface<INameMapTree> &Target);


};

UCLASS()
class KISHIVISUALNOVEL_API UNameMapTreeLibrary : public UArrayTreeLibrary
{
    GENERATED_BODY()
public:
    /*
    Interface Proxy Functions
    */
    static TKishiScriptInterface<INameMapTree> IGetChildByName(const TKishiScriptInterface<INameMapTree> &Target, FName name);
    static TArray<FName> IGetNames(const TKishiScriptInterface<INameMapTree> &Target);

    //Library
    UFUNCTION(BlueprintPure)
    static FName GetName(const TScriptInterface<INameMapTree> &Target);


    UFUNCTION(BlueprintPure)
    static bool NameToIndex(const TScriptInterface<INameMapTree> &Target,FName name,uint8 &index);
    UFUNCTION(BlueprintPure)
    static bool IndexToName(const TScriptInterface<INameMapTree> &Target,uint8 index,FName& name);
    UFUNCTION(BlueprintPure)
    static bool NamePathToIndexPath(const TScriptInterface<INameMapTree> &Target,TArray<FName> namePath,TArray<uint8> &indexPath);
    UFUNCTION(BlueprintPure)
    static bool IndexPathToNamePath(const TScriptInterface<INameMapTree> &Target,TArray<uint8> indexPath,TArray<FName> &namePath);

    


    //Libray


protected:
private:
};