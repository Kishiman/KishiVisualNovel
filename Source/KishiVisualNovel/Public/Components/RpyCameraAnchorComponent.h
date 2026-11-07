#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Interfaces/RpyCameraAnchor.h"
#include "RpyCameraAnchorComponent.generated.h"

class UCameraComponent;

/**
 * Component counterpart of ARpyCameraAnchorActorBase. Marks the point a scene "layer" attaches the VN camera to, on any actor.
 * URpyCameraAnchorLibrary::FindAnchorByLayerName finds it by AnchorLayer, and the scene manager attaches the camera to this component.
 */
UCLASS(ClassGroup = (VisualNovel), meta = (BlueprintSpawnableComponent, DisplayName = "VN Camera Anchor Component"), BlueprintType, Blueprintable)
class KISHIVISUALNOVEL_API URpyCameraAnchorComponent : public USceneComponent, public IRpyCameraAnchor
{
	GENERATED_BODY()

public:
	URpyCameraAnchorComponent();

	virtual AActor *GetActor_Implementation() const override;
	virtual UCameraComponent *GetCameraComponent_Implementation() const override;
	virtual FName GetAnchorLayer_Implementation() const override;

	/** Overrides the camera returned by GetCameraComponent. Pass None to go back to the automatic lookup. */
	UFUNCTION(BlueprintCallable, Category = "VN Camera Anchor Component")
	void SetCameraComponent(UCameraComponent *InCamera) { Camera = InCamera; }

protected:
	/** Layer name that scene commands use, as in <scene layer="student3" />. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VN Camera Anchor Component")
	FName AnchorLayer = TEXT("master");
	/** Camera of this anchor. When unset, the first camera attached below this component is used, then the owner's first camera. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "VN Camera Anchor Component")
	UCameraComponent *Camera = nullptr;
};
