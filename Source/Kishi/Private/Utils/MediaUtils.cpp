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

#include "Engine/World.h"
#include "Engine/GameViewportClient.h"

UTakeScreenshotAsyncAction *UTakeScreenshotAsyncAction::TakeScreenshot(UObject *WorldContextObject, FName TagToHide)
{
    UTakeScreenshotAsyncAction *Node = NewObject<UTakeScreenshotAsyncAction>();
    Node->WorldContextObject = WorldContextObject;
    Node->TagToHide = TagToHide;
    return Node;
}

void UTakeScreenshotAsyncAction::Activate()
{
    UWorld *World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) : nullptr;
    if (!World)
    {
        OnCompleted.Broadcast(TArray<uint8>());
        SetReadyToDestroy();
        return;
    }

    auto Future = UMediaUtils::TakeScreenshotAsync(World, TagToHide);
    // Async wait (e.g. in another task)
    Async(EAsyncExecution::Thread, [this, Future = MoveTemp(Future)]() mutable
          {
        auto PNGData = Future.Get(); // blocks until screenshot ready
        OnCompleted.Broadcast(PNGData);
        SetReadyToDestroy();
        return; });
    // Launch the async screenshot capture
    // UMediaUtils::TakeScreenshotAsync(World, TagToHide).Then([this](const TArray<uint8> &PNGData)
    //                                                         {
    //         // Must fire delegate on game thread
    //         AsyncTask(ENamedThreads::GameThread, [this, PNGData]()
    //         {
    //             OnCompleted.Broadcast(PNGData);
    //             SetReadyToDestroy();
    //         }); });
}

void TakeDeferedScreenshot(UWorld *World, TFunction<void(const TArray<uint8> &)> Callback)
{
    if (!GEngine || !GEngine->GameViewport)
        return;

    FViewport *Viewport = GEngine->GameViewport->Viewport;
    if (!Viewport)
        return;

    // Register delegate to capture AFTER render
    FDelegateHandle Handle;
    Handle = GEngine->GameViewport->OnScreenshotCaptured().AddLambda(
        [Callback, Handle](int32 Width, int32 Height, const TArray<FColor> &Bitmap)
        {
            TArray<uint8> PNGData;
            FImageUtils::CompressImageArray(Width, Height, Bitmap, PNGData);

            if (Callback)
            {
                Callback(PNGData); // Return data to caller
            }
            GEngine->GameViewport->OnScreenshotCaptured().Remove(Handle); // Unregister delegate
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
        // MediaTexture->SetTextureParameterValue(nullptr);
        MediaTexture->UpdateResource();
    }
}

void UMediaUtils::PlaySourceMedia(UMediaPlayer *MediaPlayer, UMediaTexture *MediaTexture, UFileMediaSource *MediaSource, bool bLoop)
{
    if (!MediaPlayer || !MediaSource)
        return;
    UMediaUtils::ClearMediaPlayerAndTexture(MediaPlayer, MediaTexture);
    MediaPlayer->SetLooping(bLoop);
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
