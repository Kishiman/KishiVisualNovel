#include "Widgets/KishiSpriteWidget.h"

#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "MediaPlayer.h"
#include "MediaSource.h"
#include "MediaTexture.h"
#include "Utils/MediaUtils.h"
#include "Blueprint/WidgetTree.h"

TSharedRef<SWidget> UKishiSpriteWidget::RebuildWidget()
{
	EnsureSpriteImage();
	return Super::RebuildWidget();
}

void UKishiSpriteWidget::EnsureSpriteImage()
{
	// Built on demand, so a sprite set before the first layout pass is not lost.
	if (!SpriteImage && WidgetTree)
	{
		SpriteImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("SpriteImage"));
		if (!WidgetTree->RootWidget)
		{
			// Draw the image at its brush size from the top-left corner instead of stretching it over the widget.
			UOverlay *Root = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("SpriteRoot"));
			if (UOverlaySlot *ImageSlot = Root->AddChildToOverlay(SpriteImage))
			{
				ImageSlot->SetHorizontalAlignment(HAlign_Left);
				ImageSlot->SetVerticalAlignment(VAlign_Top);
			}
			WidgetTree->RootWidget = Root;
		}
	}
}

void UKishiSpriteWidget::ApplySpriteBrush(UObject* Resource, FVector2D SourceSize, FVector2D TargetSize, EScalingMethod ScalingMethod)
{
	EnsureSpriteImage();
	if (!IsValid(SpriteImage) || !IsValid(Resource))
	{
		if (IsValid(SpriteImage))
		{
			SpriteImage->SetOpacity(0.0f);
		}
		return;
	}

	FSlateBrush Brush;
	Brush.SetResourceObject(Resource);
	Brush.ImageSize = UVectorUtils::ScaleResolutionScreen(SourceSize, TargetSize, ScalingMethod);
	if (!(Brush.ImageSize.X >= 1.0f && Brush.ImageSize.Y >= 1.0f))
	{
		// A zero, tiny or invalid target size scales the image to nothing; draw it at its own size instead.
		UE_LOG(LogTemp, Warning, TEXT("%s: scaling %s to %s with %s gives an empty image; using its source size %s."), *GetName(), *Resource->GetName(),
					 *TargetSize.ToString(), *UEnum::GetValueAsString(ScalingMethod), *SourceSize.ToString());
		Brush.ImageSize = SourceSize;
	}
	SpriteImage->SetBrush(Brush);
	SpriteImage->SetOpacity(1.0f);
}

void UKishiSpriteWidget::SetSprite(UPaperSprite* InSprite, FVector2D TargetSize, EScalingMethod ScalingMethod)
{
	UnsetFileMediaSource();
	Sprite = InSprite;
	MaxSize = TargetSize;
	Method = ScalingMethod;
	if (!IsValid(Sprite) || !IsValid(Sprite->GetBakedTexture()))
	{
		UE_LOG(LogTemp, Warning, TEXT("[VNShow] %s: SetSprite %s has no baked texture; the image is hidden."), *GetName(), *GetNameSafe(Sprite));
		if (IsValid(SpriteImage)) SpriteImage->SetOpacity(0.0f);
		return;
	}
	const FVector2D SourceSize(Sprite->GetBakedTexture()->GetSizeX(), Sprite->GetBakedTexture()->GetSizeY());
	ApplySpriteBrush(Sprite, SourceSize, TargetSize, ScalingMethod);
	// Temporary show lifecycle tracing. Filter the Output Log on "[VNShow]".
	UE_LOG(LogTemp, Log, TEXT("[VNShow] %s: SetSprite %s source=%s target=%s method=%s brush=%s image=%s"), *GetName(), *Sprite->GetName(), *SourceSize.ToString(),
				 *TargetSize.ToString(), *UEnum::GetValueAsString(ScalingMethod),
				 IsValid(SpriteImage) ? *SpriteImage->Brush.ImageSize.ToString() : TEXT("None"), *GetNameSafe(SpriteImage));
}

void UKishiSpriteWidget::SetFileMediaSource(UFileMediaSource* InMediaSource, FVector2D TargetSize, EScalingMethod ScalingMethod)
{
	UnsetSprite();
	UnsetFileMediaSource();
	FileMediaSource = InMediaSource;
	MaxSize = TargetSize;
	Method = ScalingMethod;
	if (!IsValid(FileMediaSource) || !IsValid(MediaPlayer) || !IsValid(MediaTexture))
	{
		FileMediaSource = nullptr;
		if (IsValid(SpriteImage)) SpriteImage->SetOpacity(0.0f);
		return;
	}
	MediaTexture->SetMediaPlayer(MediaPlayer);
	ApplySpriteBrush(MediaTexture, FVector2D(MediaTexture->GetSurfaceWidth(), MediaTexture->GetSurfaceHeight()), TargetSize, ScalingMethod);
	if (MediaPlayer->OpenSource(FileMediaSource))
	{
		MediaPlayer->Play();
	}
	else
	{
		UnsetFileMediaSource();
	}
}

void UKishiSpriteWidget::UnsetSprite()
{
	Sprite = nullptr;
}

void UKishiSpriteWidget::UnsetFileMediaSource()
{
	if (IsValid(MediaPlayer) && MediaPlayer->IsPlaying())
	{
		MediaPlayer->Pause();
	}
	if (IsValid(MediaPlayer) && IsValid(MediaTexture))
	{
		UMediaUtils::ClearMediaPlayerAndTexture(MediaPlayer, MediaTexture);
	}
	FileMediaSource = nullptr;
}
