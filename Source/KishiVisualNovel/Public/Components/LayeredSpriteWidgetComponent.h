#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "LayeredSpriteWidgetComponent.generated.h"

class ULayeredSprite;

/**
 * Shows a layered sprite in the world on any actor, turned to face the owner's camera at BeginPlay when it has one.
 * Set Widget Class to a widget that implements ILayeredSpriteHandler. For an interactive character, add a VN Script Interaction component beside it.
 */
UCLASS(ClassGroup = (VisualNovel), meta = (BlueprintSpawnableComponent, DisplayName = "Layered Sprite Widget Component"), BlueprintType, Blueprintable)
class KISHIVISUALNOVEL_API ULayeredSpriteWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	virtual void InitWidget() override;

	UFUNCTION(BlueprintCallable, Category = "Layered Sprite Widget Component")
	void SetLayeredSprite(ULayeredSprite *InLayeredSprite);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layered Sprite Widget Component")
	ULayeredSprite *LayeredSprite = nullptr;

private:
	void ApplySprite();
};
