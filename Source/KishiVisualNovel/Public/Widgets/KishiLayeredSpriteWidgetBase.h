#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/LayeredSpriteHandler.h"
#include "Assets/LayeredSprite.h"
#include "Animation/AnimationSolverVector2D.h"
#include "Rpy/RpySceneTypes.h"
#include "KishiLayeredSpriteWidgetBase.generated.h"

class UCanvasPanel;
class UPanelWidget;
class UPaperSprite;
class URpyTransitionBase;

/**
 * Native, Blueprintable implementation of the layered-sprite handler contract.
 * A Blueprint subclass supplies its Canvas panel and the project-specific
 * sprite widget class (WGT_Sprite in the existing content).
 */
UCLASS(Abstract, Blueprintable)
class KISHIVISUALNOVEL_API UKishiLayeredSpriteWidgetBase : public UUserWidget, public ILayeredSpriteHandler
{
	GENERATED_BODY()

public:
	virtual ULayeredSprite *GetSprite_Implementation() const override;
	virtual void SetSprite_Implementation(ULayeredSprite *Sprite) override;
	virtual bool GetLayerDisplay_Implementation(FName LayerName) const override;
	virtual void SetLayerDisplay_Implementation(FName LayerName, bool bValue) override;

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Layered Sprite")
	void SetLayeredSprite(ULayeredSprite *Sprite);

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Layered Sprite")
	void LoadLayers();

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Layered Sprite")
	void ClearLayers();

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Layered Sprite")
	UUserWidget *AddLayer(FName Name, UPaperSprite *Sprite, int32 ZOrder, FVector2D Position);

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Layered Sprite")
	void ShowLayer(FName LayerName, const FRpySceneOptions &SceneOptions, float Delay = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Layered Sprite")
	void HideLayer(FName LayerName, const FRpySceneOptions &SceneOptions, float Delay = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Layered Sprite")
	void DisplayLayersByAttribute(const FString &Attribute, TArray<FName> &NamesToShow, TArray<FName> &NamesToHide);

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Layered Sprite")
	void UpdateVisemeLayer(EMouthViseme Viseme, float TimeOffset = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Layered Sprite")
	void ResetNeutralVisemeLayer();

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Layered Sprite")
	void TransitionLayers(const FRpySceneOptions &SceneOptions);

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Layered Sprite")
	void HideShowTransition(const FRpySceneOptions &SceneOptions, const TArray<FName> &NamesToShow, const TArray<FName> &NamesToHide);

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Layered Sprite")
	void SyncTransition(const FRpySceneOptions &SceneOptions, const TArray<FName> &NamesToShow, const TArray<FName> &NamesToHide);

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Animation")
	void StartMove(FVector2D MoveTo, float Speed, EInterpolationMethod Method);

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Animation")
	void SuperTick(float DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Animation")
	FRpySceneOptions PlayMove(const FRpySceneOptions &SceneOptions);

	UFUNCTION(BlueprintPure, Category = "Visual Novel|Layered Sprite")
	UPanelWidget *GetPanelWidget() const;

	/** Called after a layer widget is created; Blueprint subclasses can customize it. */
	UFUNCTION(BlueprintNativeEvent, Category = "Visual Novel|Layered Sprite")
	void InitializeLayerWidget(UUserWidget *LayerWidget, FName LayerName, UPaperSprite *Sprite, FVector2D TargetSize);
	virtual void InitializeLayerWidget_Implementation(UUserWidget *LayerWidget, FName LayerName, UPaperSprite *Sprite, FVector2D TargetSize);

	/** Blueprint hook for applying the KishiWidget transition animations to a layer. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Visual Novel|Layered Sprite")
	void OnLayerDisplayChanged(UUserWidget *LayerWidget, bool bDisplayed, const FRpySceneOptions &SceneOptions, float Delay);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry &MyGeometry, float InDeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Novel|Layered Sprite")
	ULayeredSprite *LayeredSprite = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Novel|Layered Sprite")
	TSubclassOf<UUserWidget> LayerWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Novel|Layered Sprite")
	FVector2D MatchSize = FVector2D(1920.0f, 1080.0f);

	UPROPERTY(BlueprintReadOnly, Category = "Visual Novel|Layered Sprite")
	TMap<FName, UUserWidget *> Layers;

	UPROPERTY(BlueprintReadOnly, Category = "Visual Novel|Layered Sprite")
	TMap<FName, bool> LayerDisplay;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel *Canvas = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Novel|Layered Sprite")
	FName NeutralVisemeLayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Novel|Layered Sprite", meta = (ClampMin = "0.0"))
	float VisemeNeutralOffset = 0.2f;

	UPROPERTY(BlueprintReadWrite, Category = "Visual Novel|Animation")
	FAnimationSolverVector2D TranslateAnimationSolver;

	UPROPERTY(Transient)
	URpyTransitionBase *ActiveTranslateTransition = nullptr;

private:
	void SetLayerWidgetSprite(UUserWidget *LayerWidget, UPaperSprite *Sprite) const;
	void SetLayerDisplayInternal(FName LayerName, bool bValue, const FRpySceneOptions &SceneOptions, float Delay, bool bPlayTransition);
	bool PlayKishiWidgetTransition(UUserWidget *LayerWidget, bool bShow, const FRpySceneOptions &SceneOptions, float Delay) const;
};
