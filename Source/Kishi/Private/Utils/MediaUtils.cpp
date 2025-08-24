#include "Utils/MediaUtils.h"

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

TArray<uint8> UMediaUtils::TakeScreenshot(UWorld *World, FName TagToHide)
{
    TArray<uint8> ScreenshotRawData;

    if (!World || !GEngine || !GEngine->GameViewport)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid World or GameViewport."));
        return ScreenshotRawData;
    }

    // Find all widgets in the viewport with the given tag
    TArray<UUserWidget*> WidgetsToHide;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(World, WidgetsToHide, UUserWidget::StaticClass(), false);
    
    TArray<ESlateVisibility> OriginalVisibilities;

    // for (UUserWidget* Widget : WidgetsToHide)
    // {
    //     if (Widget && Widget->Tags.Contains(TagToHide))
    //     {
    //         OriginalVisibilities.Add(Widget->GetVisibility());
    //         Widget->SetVisibility(ESlateVisibility::Hidden);
    //     }
    //     else
    //     {
    //         OriginalVisibilities.Add(ESlateVisibility::Visible); // Placeholder
    //     }
    // }

    // Capture the screen
    FViewport* Viewport = GEngine->GameViewport->Viewport;
    FIntPoint Size = Viewport->GetSizeXY();
    TArray<FColor> Bitmap;

    if (Viewport->ReadPixels(Bitmap))
    {
        // Flip vertically (ReadPixels returns upside down)
        for (int32 Row = 0; Row < Size.Y / 2; Row++)
        {
            int32 IndexA = Row * Size.X;
            int32 IndexB = (Size.Y - Row - 1) * Size.X;

            for (int32 Col = 0; Col < Size.X; Col++)
            {
                Swap(Bitmap[IndexA + Col], Bitmap[IndexB + Col]);
            }
        }

        // 3️⃣ Convert to PNG byte array
        FImageUtils::CompressImageArray(Size.X, Size.Y, Bitmap, ScreenshotRawData);
    }

    // Restore widget visibility
    // int32 Index = 0;
    // for (UUserWidget* Widget : WidgetsToHide)
    // {
    //     if (Widget && Widget->Tags.Contains(TagToHide))
    //     {
    //         Widget->SetVisibility(OriginalVisibilities[Index]);
    //     }
    //     Index++;
    // }

    return ScreenshotRawData;
}

UTexture2D *UMediaUtils::CreateTextureFromPNG(const TArray<uint8> &PNGData)
{
    if (PNGData.Num() == 0) return nullptr;

    // Load ImageWrapper module
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

    // Decode PNG
    if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(PNGData.GetData(), PNGData.Num()))
    {
        TArray<uint8> UncompressedRGBA;
        if (ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, UncompressedRGBA))
        {
            // Create transient texture
            UTexture2D* Texture = UTexture2D::CreateTransient(
                ImageWrapper->GetWidth(),
                ImageWrapper->GetHeight(),
                PF_R8G8B8A8
            );

            if (!Texture) return nullptr;

            // Copy data to texture
            void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
            FMemory::Memcpy(TextureData, UncompressedRGBA.GetData(), UncompressedRGBA.Num());
            Texture->PlatformData->Mips[0].BulkData.Unlock();

            Texture->UpdateResource();

            return Texture;
        }
    }

    return nullptr;
}
