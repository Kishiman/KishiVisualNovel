#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/KishiScriptInterface.h"

#include "BaseGraph.h"

#include "DiGraph.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API UDiGraph : public UBaseGraph
{
    GENERATED_BODY()
};
class KISHIVISUALNOVEL_API IDiGraph : public IBaseGraph
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Graph|Directed")
    TArray<TScriptInterface<IDiGraph>> GetInNodes() const;
    virtual TArray<TScriptInterface<IDiGraph>> GetInNodes_Implementation() const = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Graph|Directed")
    TArray<TScriptInterface<IDiGraph>> GetOutNodes() const;
    virtual TArray<TScriptInterface<IDiGraph>> GetOutNodes_Implementation() const = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Graph|Directed")
    uint8 GetInNodesSize() const;
    virtual uint8 GetInNodesSize_Implementation() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Graph|Directed")
    uint8 GetOutNodesSize() const;
    virtual uint8 GetOutNodesSize_Implementation() const;

    virtual TArray<TScriptInterface<IBaseGraph>> GetNodes_Implementation() const override;
    virtual uint8 GetNodesSize_Implementation() const override;
};

UCLASS(MinimalAPI)
class UDiGraphImplementation : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    /*
    Default Implementation
    */

    UFUNCTION(BlueprintPure, meta = (DefaultToSelf = "Target"), Category = "Default|Graph|Directed")
    static uint8 IGetInNodesSize_Default(const TScriptInterface<IDiGraph> &Target);
    UFUNCTION(BlueprintPure, meta = (DefaultToSelf = "Target"), Category = "Default|Graph|Directed")
    static uint8 IGetOutNodesSize_Default(const TScriptInterface<IDiGraph> &Target);
    /*
    Overrides Implementation
    */
    UFUNCTION(BlueprintPure, meta = (DefaultToSelf = "Target"), Category = "Default|Graph|Directed")
    static TArray<TScriptInterface<IBaseGraph>> IGetNodes_Default(const TScriptInterface<IDiGraph> &Target);
    UFUNCTION(BlueprintPure, meta = (DefaultToSelf = "Target"), Category = "Default|Graph|Directed")
    static uint8 IGetNodesSize_Default(const TScriptInterface<IDiGraph> &Target);
};

UCLASS()
class KISHIVISUALNOVEL_API UDiGraphLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    /*
    Interface Proxy Functions
    */

    static TArray<TKishiScriptInterface<IDiGraph>> IGetInNodes(const TKishiScriptInterface<IDiGraph> &Target);
    static TArray<TKishiScriptInterface<IDiGraph>> IGetOutNodes(const TKishiScriptInterface<IDiGraph> &Target);
    static uint8 IGetInNodesSize(const TKishiScriptInterface<IDiGraph> &Target);
    static uint8 IGetOutNodesSize(const TKishiScriptInterface<IDiGraph> &Target);

    /*
    Library
    */

protected:
private:
};