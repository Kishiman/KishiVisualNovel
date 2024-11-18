#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LayeredSpriteHandler.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API ULayeredSpriteHandler : public UInterface
{
    GENERATED_BODY()
};

class KISHIVISUALNOVEL_API ILayeredSpriteHandler
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    ULayeredSprite *GetSprite() const;
    virtual ULayeredSprite *GetSprite_Implementation() const = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetSprite(ULayeredSprite *Sprite);
    virtual void SetSprite_Implementation(ULayeredSprite *Sprite) = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool GetLayerDisplay(FName LayerName) const;
    virtual bool GetLayerDisplay_Implementation(FName LayerName) const = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetLayerDisplay(FName LayerName, bool Value);
    virtual void SetLayerDisplay_Implementation(FName LayerName, bool Value) = 0;
};

UCLASS()
class KISHIVISUALNOVEL_API ULayeredSpriteHandlerLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure)
    static void GetLayersToDisplayByAttribute(const TScriptInterface<ILayeredSpriteHandler> &Target, FString Attribute, TArray<FName> &NamesToShow, TArray<FName> &NamesToHide);

    UFUNCTION(BlueprintCallable)
    static void SetLayersDisplayByAttribute(const TScriptInterface<ILayeredSpriteHandler> &Target, FString Attribute, TArray<FName> &ShownNames, TArray<FName> &HidenNames);

protected:
private:
};
