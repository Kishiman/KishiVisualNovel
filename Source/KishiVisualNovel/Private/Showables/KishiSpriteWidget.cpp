#include "Showables/KishiSpriteWidget.h"
#include "Showables/VideoMediaSprite.h"

#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "FileMediaSource.h"
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

bool UKishiSpriteWidget::SetShowableAsset_Implementation(UObject* Asset, FVector2D TargetSize)
{
	if (UPaperSprite* PaperSprite = Cast<UPaperSprite>(Asset))
	{
		SetSprite(PaperSprite, TargetSize, Method);
		return true;
	}
	if (UVideoMediaSprite* Video = Cast<UVideoMediaSprite>(Asset))
	{
		if (!IsValid(Video->fileMediaSource))
		{
			UE_LOG(LogTemp, Error, TEXT("%s: video media sprite %s has no file media source."), *GetName(), *Video->GetName());
			return false;
		}
		SetFileMediaSource(Video->fileMediaSource, TargetSize, Method);
		return true;
	}
	UE_LOG(LogTemp, Error, TEXT("%s: cannot display %s; a sprite widget shows Paper2D sprites and video media sprites."), *GetName(), *GetNameSafe(Asset));
	return false;
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

bool UKishiSpriteWidget::EnsureMediaPlayer()
{
	if (!IsValid(MediaPlayer))
	{
		MediaPlayer = NewObject<UMediaPlayer>(this, NAME_None, RF_Transient);
		MediaPlayer->PlayOnOpen = true;
	}
	if (!IsValid(MediaTexture))
	{
		MediaTexture = NewObject<UMediaTexture>(this, NAME_None, RF_Transient);
		// Transparent until the first frame arrives, instead of a black flash.
		MediaTexture->AutoClear = true;
		MediaTexture->ClearColor = FLinearColor::Transparent;
		MediaTexture->SetMediaPlayer(MediaPlayer);
		MediaTexture->UpdateResource();
	}
	MediaPlayer->OnMediaOpened.AddUniqueDynamic(this, &UKishiSpriteWidget::HandleMediaOpened);
	MediaPlayer->OnEndReached.AddUniqueDynamic(this, &UKishiSpriteWidget::HandleMediaEndReached);
	return IsValid(MediaPlayer) && IsValid(MediaTexture);
}

void UKishiSpriteWidget::SetFileMediaSource(UFileMediaSource* InMediaSource, FVector2D TargetSize, EScalingMethod ScalingMethod)
{
	UnsetSprite();
	UnsetFileMediaSource();
	FileMediaSource = InMediaSource;
	MaxSize = TargetSize;
	Method = ScalingMethod;
	if (!IsValid(FileMediaSource) || !EnsureMediaPlayer())
	{
		UE_LOG(LogTemp, Error, TEXT("%s: cannot play media source %s."), *GetName(), *GetNameSafe(InMediaSource));
		FileMediaSource = nullptr;
		if (IsValid(SpriteImage)) SpriteImage->SetOpacity(0.0f);
		return;
	}
	MediaTexture->SetMediaPlayer(MediaPlayer);
	// Transparent until the new video's first frame (HandleMediaOpened turns clearing off once it plays).
	MediaTexture->AutoClear = true;
	// The video's size is known once it opens (HandleMediaOpened); until then use the target size, so the image is laid out at its usual aspect.
	const FVector2D SurfaceSize(MediaTexture->GetSurfaceWidth(), MediaTexture->GetSurfaceHeight());
	ApplySpriteBrush(MediaTexture, SurfaceSize.X >= 1.0f && SurfaceSize.Y >= 1.0f ? SurfaceSize : TargetSize, TargetSize, ScalingMethod);
	// Only a whole-video loop uses the player's looping; a tail loop and a single play are handled in NativeTick.
	MediaPlayer->SetLooping(MediaLoop < 0.0f);
	if (MediaPlayer->OpenSource(FileMediaSource))
	{
		MediaPlayer->Play();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s: could not open media source %s (%s)."), *GetName(), *FileMediaSource->GetName(), *FileMediaSource->GetFilePath());
		UnsetFileMediaSource();
	}
}

void UKishiSpriteWidget::HandleMediaOpened(FString OpenedUrl)
{
	if (!IsValid(FileMediaSource) || !IsValid(MediaPlayer) || !IsValid(MediaTexture))
		return;
	// Keep the last frame on screen while the player has none, such as during a loop's seek; clearing shows what is behind the video for a frame.
	MediaTexture->AutoClear = false;
	const FIntPoint Dimensions = MediaPlayer->GetVideoTrackDimensions(INDEX_NONE, INDEX_NONE);
	if (Dimensions.X > 0 && Dimensions.Y > 0)
		ApplySpriteBrush(MediaTexture, FVector2D(Dimensions.X, Dimensions.Y), MaxSize, Method);
	// Opening resets looping on some players.
	MediaPlayer->SetLooping(MediaLoop < 0.0f);
}

void UKishiSpriteWidget::Show_Implementation(const FKishiShowableOptions& Options)
{
	Super::Show_Implementation(Options);
	MediaLoop = Options.MediaLoop;
	if (IsValid(FileMediaSource) && IsValid(MediaPlayer))
		MediaPlayer->SetLooping(MediaLoop < 0.0f);
}

bool UKishiSpriteWidget::IsLoading_Implementation() const
{
	// The texture takes the video's size from its first frame; before that it is empty or a 2x2 clear.
	return IsValid(FileMediaSource) && IsValid(MediaTexture) && (MediaTexture->GetSurfaceWidth() <= 2.0f || MediaTexture->GetSurfaceHeight() <= 2.0f);
}

void UKishiSpriteWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateMediaPlayback(false);
}

void UKishiSpriteWidget::HandleMediaEndReached()
{
	UpdateMediaPlayback(true);
}

FTimespan UKishiSpriteWidget::GetMediaEndMargin() const
{
	// Stop a frame and a half before the end, so the player never reaches it and clears the image.
	const float FrameRate = IsValid(MediaPlayer) ? MediaPlayer->GetVideoTrackFrameRate(INDEX_NONE, INDEX_NONE) : 0.0f;
	return FTimespan::FromSeconds(FrameRate > 0.0f ? 1.5f / FrameRate : 0.05f);
}

void UKishiSpriteWidget::UpdateMediaPlayback(bool bReachedEnd)
{
	if (!IsValid(FileMediaSource) || !IsValid(MediaPlayer) || MediaLoop < 0.0f)
		return;
	if (!bReachedEnd && !MediaPlayer->IsPlaying())
		return;
	const FTimespan Duration = MediaPlayer->GetDuration();
	if (Duration <= FTimespan::Zero())
		return;
	const FTimespan Margin = GetMediaEndMargin();
	if (!bReachedEnd && MediaPlayer->GetTime() < Duration - Margin)
		return;
	if (MediaLoop > 0.0f)
	{
		const FTimespan LoopStart = FMath::Max(FTimespan::Zero(), Duration - FTimespan::FromSeconds(MediaLoop));
		MediaPlayer->Seek(LoopStart);
		if (bReachedEnd)
			MediaPlayer->Play();
		return;
	}
	// Played once: hold the last frame.
	if (bReachedEnd)
		MediaPlayer->Seek(FMath::Max(FTimespan::Zero(), Duration - Margin));
	MediaPlayer->Pause();
}

void UKishiSpriteWidget::NativeDestruct()
{
	// Stop decoding once the widget leaves the screen, such as a background replaced by the next scene.
	UnsetFileMediaSource();
	Super::NativeDestruct();
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
