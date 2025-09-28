#include "Utils/MediaUtils.h"

#include "CoreMinimal.h"

#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "ImageUtils.h" // For PNG export
#include "Engine/Texture2D.h"
#include "Misc/FileHelper.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"

#include "HighResScreenshot.h"
#include "Async/Async.h"
#include "Async/Future.h"
#include "Slate/SceneViewport.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SViewport.h"

void TakeDeferedScreenshot(UWorld *World, TFunction<void(const TArray<uint8> &)> Callback)
{
    if (!GEngine || !GEngine->GameViewport)
        return;

    FViewport *Viewport = GEngine->GameViewport->Viewport;
    if (!Viewport)
        return;

    // Register delegate to capture AFTER render
    GEngine->GameViewport->OnScreenshotCaptured().AddLambda(
        [Callback](int32 Width, int32 Height, const TArray<FColor> &Bitmap)
        {
            TArray<uint8> PNGData;
            FImageUtils::CompressImageArray(Width, Height, Bitmap, PNGData);

            if (Callback)
            {
                Callback(PNGData); // Return data to caller
            }
        });

    // Trigger screenshot
    FScreenshotRequest::RequestScreenshot(false);
}

void UMediaUtils::ClearMediaPlayerAndTexture(UMediaPlayer *MediaPlayer, UMediaTexture *MediaTexture)
{
    if (MediaPlayer)
    {
        MediaPlayer->Close();
        MediaPlayer->SetLooping(false);
    }

    if (MediaTexture)
    {
        MediaTexture->UpdateResource();
    }
}

void UMediaUtils::PlaySourceMedia(UMediaPlayer *MediaPlayer, UMediaTexture *MediaTexture, UFileMediaSource *MediaSource)
{
    UMediaUtils::ClearMediaPlayerAndTexture(MediaPlayer, MediaTexture);
    MediaPlayer->OpenSource(MediaSource);
}

TFuture<TArray<uint8>> UMediaUtils::TakeScreenshotAsync(UWorld *World, FName TagToHide)
{
    TSharedPtr<TPromise<TArray<uint8>>> Promise = MakeShared<TPromise<TArray<uint8>>>();
    TFuture<TArray<uint8>> Future = Promise->GetFuture();

    if (!World || !GEngine || !GEngine->GameViewport)
    {
        Promise->SetValue(TArray<uint8>());
        return Future;
    }

    // Schedule screenshot on game thread
    AsyncTask(ENamedThreads::GameThread, [World, TagToHide, Promise]()
              {
        TArray<uint8> PNGData;

        // Grab Slate widget
        TSharedPtr<SViewport> GameViewportWidget = GEngine->GameViewport->GetGameViewportWidget();
        if (!GameViewportWidget.IsValid())
        {
            Promise->SetValue(PNGData);
            return;
        }

        TSharedPtr<SWidget> Widget = (TSharedPtr<SWidget>)(GameViewportWidget);
        TSharedRef<SWidget> WidgetRef = Widget.ToSharedRef();

        TArray<FColor> OutPixels;
        FIntVector Size;

        FSlateApplication::Get().TakeScreenshot(WidgetRef, OutPixels, Size);

        if (OutPixels.Num() > 0)
        {
            FIntPoint Size2D(Size.X, Size.Y);
            FImageUtils::CompressImageArray(Size2D.X, Size2D.Y, OutPixels, PNGData);
        }

        Promise->SetValue(PNGData); });

    return Future;
    return Async(EAsyncExecution::ThreadPool, [World, TagToHide]()
                 {
            TArray<uint8> PNGData;

            if (!World || !GEngine || !GEngine->GameViewport)
            {
                return PNGData;
            }

            // Grab Slate widget representing the game viewport
            TSharedPtr<SViewport> GameViewportWidget = (GEngine->GameViewport->GetGameViewportWidget());
            if (!GameViewportWidget.IsValid())
            {
                return PNGData;
            }
            TSharedPtr<SWidget> Widget = (TSharedPtr<SWidget>)(GameViewportWidget);
            TSharedRef<SWidget> WidgetRef = Widget.ToSharedRef();

            // Prepare output
            TArray<FColor> OutPixels;
            FIntVector Size;

            // Take screenshot of full viewport (includes Slate/UMG widgets)
            FSlateApplication::Get().TakeScreenshot(WidgetRef, OutPixels, Size);
            if (OutPixels.Num() > 0)
            {
                // Convert to PNG
                FIntPoint Size2D(Size.X, Size.Y);
                FImageUtils::CompressImageArray(Size2D.X, Size2D.Y, OutPixels, PNGData);
            }

            return PNGData; });
}

UTexture2D *UMediaUtils::CreateTextureFromPNG(const TArray<uint8> &PNGData)
{
    if (PNGData.Num() == 0)
        return nullptr;

    // Load ImageWrapper module
    IImageWrapperModule &ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

    // Decode PNG
    if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(PNGData.GetData(), PNGData.Num()))
    {
        TArray<uint8> UncompressedRGBA;
        if (ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, UncompressedRGBA))
        {
            // Create transient texture
            UTexture2D *Texture = UTexture2D::CreateTransient(
                ImageWrapper->GetWidth(),
                ImageWrapper->GetHeight(),
                PF_R8G8B8A8);

            if (!Texture)
                return nullptr;

            // Copy data to texture
            void *TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
            FMemory::Memcpy(TextureData, UncompressedRGBA.GetData(), UncompressedRGBA.Num());
            Texture->PlatformData->Mips[0].BulkData.Unlock();

            Texture->UpdateResource();

            return Texture;
        }
    }

    return nullptr;
}
