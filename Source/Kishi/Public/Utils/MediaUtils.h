#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "FileMediaSource.h"
#include "MediaPlayer.h"
#include "MediaTexture.h"
#include "MediaSource.h"

#include "MediaUtils.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTakeScreenshotResult, const TArray<uint8> &, PNGData);

UCLASS()
class KISHI_API UTakeScreenshotAsyncAction : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    // Called when screenshot is complete
    UPROPERTY(BlueprintAssignable)
    FTakeScreenshotResult OnCompleted;

    // Start screenshot
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
    static UTakeScreenshotAsyncAction *TakeScreenshot(UObject *WorldContextObject, FName TagToHide);

    virtual void Activate() override;

private:
    void HandleScreenshotCaptured(const TArray<uint8> &PNGData);

    UPROPERTY()
    UObject *WorldContextObject;

    FName TagToHide;
};

UCLASS()
class KISHI_API UMediaUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable)
    static void ClearMediaPlayerAndTexture(UMediaPlayer *MediaPlayer, UMediaTexture *MediaTexture);
    UFUNCTION(BlueprintCallable)
    static void PlaySourceMedia(UMediaPlayer *MediaPlayer, UMediaTexture *MediaTexture, UFileMediaSource *MediaSource, bool bLoop = false);
    UFUNCTION(BlueprintCallable)
    static UTexture2D *CreateTextureFromPNG(const TArray<uint8> &PNGData);

    static TFuture<TArray<uint8>> TakeScreenshotAsync(UWorld *World, FName TagToHide);

protected:
private:
};