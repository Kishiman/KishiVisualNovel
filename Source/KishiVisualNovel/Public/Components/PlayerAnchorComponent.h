#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Interfaces/PlayerAnchor.h"
#include "PlayerAnchorComponent.generated.h"

class ACharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerAnchorChanged, ACharacter *, PlayerCharacter);

/**
 * Component counterpart of APlayerAnchorActorBase. Place it on any actor (a chair, a terminal) at the spot the player snaps to.
 * The anchor transform is PlayerAnchorTransform relative to this component, so moving the component moves the anchor.
 * Find it on an actor with UPlayerAnchorLibrary::FindPlayerAnchor.
 */
UCLASS(ClassGroup = (VisualNovel), meta = (BlueprintSpawnableComponent, DisplayName = "Player Anchor Component"), BlueprintType, Blueprintable)
class KISHIVISUALNOVEL_API UPlayerAnchorComponent : public USceneComponent, public IPlayerAnchor
{
	GENERATED_BODY()

public:
	UPlayerAnchorComponent();

	virtual void OnEnterAnchor_Implementation(ACharacter *PlayerCharacter) override;
	virtual void OnExitAnchor_Implementation(ACharacter *PlayerCharacter) override;
	virtual FTransform GetAnchorTransform_Implementation() const override;
	virtual void GetViewLimits_Implementation(float &OutMinYaw, float &OutMaxYaw, float &OutMinPitch, float &OutMaxPitch) const override;
	virtual bool CanAnchor_Implementation(ACharacter *PlayerCharacter) const override;

	/** Optional. Broadcast when a player starts using this anchor. */
	UPROPERTY(BlueprintAssignable, Category = "Player Anchor Component|Hooks")
	FOnPlayerAnchorChanged OnAnchorEntered;
	/** Optional. Broadcast when a player stops using this anchor. */
	UPROPERTY(BlueprintAssignable, Category = "Player Anchor Component|Hooks")
	FOnPlayerAnchorChanged OnAnchorExited;

	/** Player currently using this anchor, or None. */
	UFUNCTION(BlueprintPure, Category = "Player Anchor Component")
	ACharacter *GetAnchoredCharacter() const { return AnchoredCharacter; }

protected:
	/** Player snap transform, relative to this component. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Anchor Component")
	FTransform PlayerAnchorTransform = FTransform::Identity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Anchor Component|View")
	float MinYaw = -45.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Anchor Component|View")
	float MaxYaw = 45.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Anchor Component|View")
	float MinPitch = -20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Anchor Component|View")
	float MaxPitch = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Anchor Component")
	bool bAnchorAvailable = true;
	/** Refuse a second player while one is anchored. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Anchor Component")
	bool bSingleOccupant = true;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Player Anchor Component")
	ACharacter *AnchoredCharacter = nullptr;
};
