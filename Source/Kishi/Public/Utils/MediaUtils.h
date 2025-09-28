#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "FileMediaSource.h"
#include "MediaPlayer.h"
#include "MediaTexture.h"
#include "MediaSource.h"

#include "MediaUtils.generated.h"

UCLASS()
class KISHI_API UMediaUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable)
    static void ClearMediaPlayerAndTexture(UMediaPlayer *MediaPlayer, UMediaTexture *MediaTexture);
    UFUNCTION(BlueprintCallable)
    static void PlaySourceMedia(UMediaPlayer *MediaPlayer, UMediaTexture *MediaTexture, UFileMediaSource *MediaSource);
    UFUNCTION(BlueprintCallable)
    static UTexture2D *CreateTextureFromPNG(const TArray<uint8> &PNGData);

    static TFuture<TArray<uint8>> TakeScreenshotAsync(UWorld *World, FName TagToHide);

protected:
private:
};