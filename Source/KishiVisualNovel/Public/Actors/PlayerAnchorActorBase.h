#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PlayerAnchor.h"
#include "PlayerAnchorActorBase.generated.h"

class USceneComponent;

/** Reusable native anchor with Blueprint hooks for project-specific interaction behavior. */
UCLASS(Abstract, Blueprintable)
class KISHIVISUALNOVEL_API APlayerAnchorActorBase : public AActor, public IPlayerAnchor
{
	GENERATED_BODY()

public:
	APlayerAnchorActorBase();
	virtual void OnEnterAnchor_Implementation(ACharacter* PlayerCharacter) override;
	virtual void OnExitAnchor_Implementation(ACharacter* PlayerCharacter) override;
	virtual FTransform GetAnchorTransform_Implementation() const override;
	virtual void GetViewLimits_Implementation(float& OutMinYaw, float& OutMaxYaw, float& OutMinPitch, float& OutMaxPitch) const override;
	virtual bool CanAnchor_Implementation(ACharacter* PlayerCharacter) const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Anchor")
	USceneComponent* AnchorRoot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anchor")
	FTransform PlayerAnchorTransform = FTransform::Identity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anchor|View")
	float MinYaw = -45.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anchor|View")
	float MaxYaw = 45.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anchor|View")
	float MinPitch = -20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anchor|View")
	float MaxPitch = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anchor")
	bool bAnchorAvailable = true;

	UFUNCTION(BlueprintImplementableEvent, Category="Anchor")
	void OnAnchorEntered(ACharacter* PlayerCharacter);
	UFUNCTION(BlueprintImplementableEvent, Category="Anchor")
	void OnAnchorExited(ACharacter* PlayerCharacter);
};
