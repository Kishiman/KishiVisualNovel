#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/KishiScriptInterface.h"
#include "BaseGraph.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API UBaseGraph : public UInterface
{
    GENERATED_BODY()
};
class KISHIVISUALNOVEL_API IBaseGraph
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Graph")
    TArray<TScriptInterface<IBaseGraph>> GetNodes() const;
    virtual TArray<TScriptInterface<IBaseGraph>> GetNodes_Implementation() const = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Graph")
    uint8 GetNodesSize() const;
    virtual uint8 GetNodesSize_Implementation() const;
};

UCLASS(MinimalAPI)
class  UBaseGraphImplementation : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    /*
    DefaultImplementation
    */
    UFUNCTION(BlueprintPure, meta = (DefaultToSelf = "Target"), Category = "Default|Graph")
    static uint8 IGetNodesSize_Default(const TScriptInterface<IBaseGraph> &Target);

    /*
    Overrides Implementation
    */
};

UCLASS()
class KISHIVISUALNOVEL_API UBaseGraphLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    /*
    Interface Proxy Functions
    */
    static TArray<TScriptInterface<IBaseGraph>> IGetNodes(TKishiScriptInterface<IBaseGraph> Target);
    static uint8 IGetNodesSize(TKishiScriptInterface<IBaseGraph> Target);
    /*
    Library
    */

protected:
private:
};