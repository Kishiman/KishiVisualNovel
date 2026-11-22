#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Utils/StringUtils.h"
#include "LayeredSpriteHandler.generated.h"

class ULayeredSprite;

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API ULayeredSpriteHandler : public UInterface
{
    GENERATED_BODY()
};

class KISHIVISUALNOVEL_API ILayeredSpriteHandler
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ILayeredSpriteHandler|Required", meta = (ToolTip = "Required: return the layered sprite this handler displays."))
    ULayeredSprite *GetSprite() const;
    virtual ULayeredSprite *GetSprite_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: ILayeredSpriteHandler::GetSprite")); return nullptr; }

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ILayeredSpriteHandler|Required", meta = (ToolTip = "Required: display Sprite, replacing the current layered sprite."))
    void SetSprite(ULayeredSprite *Sprite);
    virtual void SetSprite_Implementation(ULayeredSprite *Sprite) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: ILayeredSpriteHandler::SetSprite")); }

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ILayeredSpriteHandler|Required", meta = (ToolTip = "Required: return whether the layer named LayerName is displayed."))
    bool GetLayerDisplay(FName LayerName) const;
    virtual bool GetLayerDisplay_Implementation(FName LayerName) const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: ILayeredSpriteHandler::GetLayerDisplay")); return false; }

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ILayeredSpriteHandler|Required", meta = (ToolTip = "Required: show or hide the layer named LayerName."))
    void SetLayerDisplay(FName LayerName, bool Value);
    virtual void SetLayerDisplay_Implementation(FName LayerName, bool Value) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: ILayeredSpriteHandler::SetLayerDisplay")); }

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ILayeredSpriteHandler|Hooks", meta = (ToolTip = "Optional: called when the speaker this sprite is shown for changes mouth viseme. The default does nothing."))
    void OnSpeakerViseme(EMouthViseme Viseme, float TimeOffset);
    virtual void OnSpeakerViseme_Implementation(EMouthViseme Viseme, float TimeOffset) {}
};

UCLASS()
class KISHIVISUALNOVEL_API ULayeredSpriteHandlerLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure)
    static void GetLayersToDisplayByAttribute(const TScriptInterface<ILayeredSpriteHandler> &Target, FString Attribute, TArray<FName> &NamesToShow, TArray<FName> &NamesToHide);

    UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Target"))
    static void SetLayersDisplayByAttribute(const TScriptInterface<ILayeredSpriteHandler> &Target, FString Attribute, TArray<FName> &ShownNames, TArray<FName> &HidenNames);

    UFUNCTION(BlueprintPure)
    static FName GetDisplayedLayerInGroup(const TScriptInterface<ILayeredSpriteHandler> &Target, FName GroupName);

protected:
private:
};
