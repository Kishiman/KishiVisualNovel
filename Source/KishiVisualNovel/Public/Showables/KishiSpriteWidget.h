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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kishi Sprite Widget")
	UMediaPlayer* MediaPlayer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kishi Sprite Widget")
	UMediaTexture* MediaTexture = nullptr;

	UPROPERTY(meta=(BindWidgetOptional))
	UImage* SpriteImage = nullptr;

private:
	void EnsureSpriteImage();
	void ApplySpriteBrush(UObject* Resource, FVector2D SourceSize, FVector2D TargetSize, EScalingMethod ScalingMethod);
};
