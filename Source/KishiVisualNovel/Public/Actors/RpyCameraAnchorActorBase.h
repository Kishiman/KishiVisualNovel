#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/RpyCameraAnchor.h"
#include "RpyCameraAnchorActorBase.generated.h"

class UCameraComponent;
class USceneComponent;

/** Native camera anchor that can be subclassed and configured in Blueprint. */
UCLASS(Abstract, Blueprintable)
class KISHIVISUALNOVEL_API ARpyCameraAnchorActorBase : public AActor, public IRpyCameraAnchor
{
	GENERATED_BODY()

public:
	ARpyCameraAnchorActorBase();
	virtual AActor* GetActor_Implementation() const override;
	virtual UCameraComponent* GetCameraComponent_Implementation() const override;
	virtual FName GetAnchorLayer_Implementation() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera Anchor")
	USceneComponent* AnchorRoot = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera Anchor")
	UCameraComponent* Camera = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Anchor")
	FName AnchorLayer = TEXT("master");
};
