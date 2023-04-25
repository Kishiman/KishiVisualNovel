#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Layerable.generated.h"




UINTERFACE()
class KISHI_API ULayerable : public UInterface
{
    GENERATED_BODY()
};
class KISHI_API ILayerable
{
    GENERATED_BODY()
public:
    /*  Set a layer to be on or off
    Layer: path+name
    */  
    UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
    void SetLayer(const FString& Layer,bool value);
    virtual void SetLayer_Implementation(const FString& Layer,bool value)=0;

    UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
    TArray<FString> GetAllLayers()const;
    virtual TArray<FString> GetAllLayers_Implementation()const=0;

    UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
    TArray<FString> GetAllSelectiveLayers()const;
    virtual TArray<FString> GetAllSelectiveLayers_Implementation()const=0;
};

UCLASS()
class KISHI_API ULayerableLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure)
    static bool HasLayer(const TScriptInterface<ILayerable>& Target,const FString& Layer);
    UFUNCTION(BlueprintPure)
    static TSet<FName> GetSelectiveLayerOptions(const TScriptInterface<ILayerable>& Target,const FString& Path);
    UFUNCTION(BlueprintCallable)
    static void SetSelectiveLayer(TScriptInterface<ILayerable>& Target,const FString& Layer);
    UFUNCTION(BlueprintCallable)
    static void SetSelectiveLayerOption(TScriptInterface<ILayerable>& Target,const FString& Path, FName Option);
protected:
private:
};