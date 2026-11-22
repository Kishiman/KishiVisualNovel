#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "LayeredSpriteWidgetComponent.generated.h"

class ULayeredSprite;

/**
 * Shows a layered sprite in the world on any actor, turned to face the owner's camera at BeginPlay when it has one.
 * Widget Class defaults to KishiLayeredSpriteWidget; any widget that implements ILayeredSpriteHandler works. For an interactive character, add a VN Script Interaction component beside it.
 */
UCLASS(ClassGroup = (VisualNovel), meta = (BlueprintSpawnableComponent, DisplayName = "Layered Sprite Widget Component"), BlueprintType, Blueprintable)
class KISHIVISUALNOVEL_API ULayeredSpriteWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	ULayeredSpriteWidgetComponent(const FObjectInitializer &ObjectInitializer);

	virtual void InitWidget() override;
	virtual void OnRegister() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent) override;
#endif

	UFUNCTION(BlueprintCallable, Category = "Layered Sprite Widget Component")
	void SetLayeredSprite(ULayeredSprite *InLayeredSprite);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layered Sprite Widget Component")
	ULayeredSprite *LayeredSprite = nullptr;

	/**
	 * Widget pixels per Unreal unit (1 unit = 1 cm). Above 0, it sets this component's scale to 1 / PixelsPerUnrealUnit,
	 * so the Draw Size keeps its resolution: at 9.6, a 1920 px wide widget spans 200 cm. At 0, the Transform scale is used.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layered Sprite Widget Component", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float PixelsPerUnrealUnit = 0.0f;

private:
	void ApplySprite();
	void ApplyPixelsPerUnrealUnit();
};
