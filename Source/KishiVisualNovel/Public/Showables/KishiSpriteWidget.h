#pragma once

#include "CoreMinimal.h"
#include "Showables/KishiShowableWidget.h"
#include "PaperSprite.h"
#include "Utils/VectorUtils.h"
#include "KishiSpriteWidget.generated.h"

class UImage;
class UFileMediaSource;
class UMediaPlayer;
class UMediaTexture;

/** Native showable widget for a Paper2D sprite or a media source. */
UCLASS(BlueprintType, Blueprintable)
class KISHIVISUALNOVEL_API UKishiSpriteWidget : public UKishiShowableWidget
{
	GENERATED_BODY()

public:
	/** Displays a Paper2D sprite or a video media sprite's media source, scaled to TargetSize with this widget's Method. */
	virtual bool SetShowableAsset_Implementation(UObject* Asset, FVector2D TargetSize) override;
	/** Also applies Options.MediaLoop to a playing media source. */
	virtual void Show_Implementation(const FKishiShowableOptions& Options) override;
	/** True while a media source has not drawn its first frame. */
	virtual bool IsLoading_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category="Kishi Sprite Widget")
	void SetSprite(UPaperSprite* InSprite, FVector2D TargetSize = FVector2D(1920.0f, 1080.0f), EScalingMethod ScalingMethod = EScalingMethod::Fit);

	UFUNCTION(BlueprintCallable, Category="Kishi Sprite Widget")
	void SetFileMediaSource(UFileMediaSource* InMediaSource, FVector2D TargetSize = FVector2D(1920.0f, 1080.0f), EScalingMethod ScalingMethod = EScalingMethod::Fit);

	UFUNCTION(BlueprintCallable, Category="Kishi Sprite Widget")
	void UnsetSprite();

	UFUNCTION(BlueprintCallable, Category="Kishi Sprite Widget")
	void UnsetFileMediaSource();

	UFUNCTION(BlueprintPure, Category="Kishi Sprite Widget")
	FName GetSpriteName() const { return Name; }

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kishi Sprite Widget")
	FName Name = TEXT("Sprite");

	UPROPERTY(BlueprintReadOnly, Category="Kishi Sprite Widget")
	UPaperSprite* Sprite = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kishi Sprite Widget")
	FVector2D MaxSize = FVector2D(1920.0f, 1080.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kishi Sprite Widget")
	EScalingMethod Method = EScalingMethod::Fit;

	UPROPERTY(BlueprintReadOnly, Category="Kishi Sprite Widget")
	UFileMediaSource* FileMediaSource = nullptr;

	/** Optional: the player for media sources. Left empty, the widget creates its own, so two videos can play at once (a background cross-fade). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kishi Sprite Widget")
	UMediaPlayer* MediaPlayer = nullptr;

	/** Optional: the texture MediaPlayer renders to. Left empty, the widget creates its own. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kishi Sprite Widget")
	UMediaTexture* MediaTexture = nullptr;

	/** 0 plays a media source once and holds its last frame; a positive value loops its last that many seconds; a negative value loops all of it. Show sets it from FKishiShowableOptions::MediaLoop. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kishi Sprite Widget")
	float MediaLoop = 0.0f;

	UPROPERTY(meta=(BindWidgetOptional))
	UImage* SpriteImage = nullptr;

	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	/** Creates MediaPlayer and MediaTexture when none were assigned. Returns false if there is still no player. */
	bool EnsureMediaPlayer();
	/** Sizes the image to the video once its dimensions are known. */
	UFUNCTION()
	void HandleMediaOpened(FString OpenedUrl);
	/** Catches the end when a frame overshot the check in NativeTick. */
	UFUNCTION()
	void HandleMediaEndReached();
	/** Before the end of the media: seeks back for a tail loop, or pauses on the last frame. */
	void UpdateMediaPlayback(bool bReachedEnd);
	FTimespan GetMediaEndMargin() const;
	void EnsureSpriteImage();
	void ApplySpriteBrush(UObject* Resource, FVector2D SourceSize, FVector2D TargetSize, EScalingMethod ScalingMethod);
};
