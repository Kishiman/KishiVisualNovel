#include "Widgets/KishiSpriteWidget.h"

#include "Components/Image.h"
#include "MediaPlayer.h"
#include "MediaSource.h"
#include "MediaTexture.h"
#include "Utils/MediaUtils.h"
#include "Blueprint/WidgetTree.h"

TSharedRef<SWidget> UKishiSpriteWidget::RebuildWidget()
{
	if (!SpriteImage && WidgetTree)
	{
		SpriteImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("SpriteImage"));
		if (!WidgetTree->RootWidget)
		{
			WidgetTree->RootWidget = SpriteImage;
		}
	}
	return Super::RebuildWidget();
}

void UKishiSpriteWidget::ApplySpriteBrush(UObject* Resource, FVector2D SourceSize, FVector2D TargetSize, EScalingMethod ScalingMethod)
{
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
		if (IsValid(SpriteImage)) SpriteImage->SetOpacity(0.0f);
		return;
	}
	const FVector2D SourceSize(Sprite->GetBakedTexture()->GetSizeX(), Sprite->GetBakedTexture()->GetSizeY());
	ApplySpriteBrush(Sprite, SourceSize, TargetSize, ScalingMethod);
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
