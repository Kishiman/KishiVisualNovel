#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/RpyMainActor.h"
#include "RpyCloseVNEventListenerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRpyCloseVNEvent);

/**
 * Component counterpart of ARpyCloseVNEventListenerBase. Lets any actor react when a VN closes, through the OnVNClosed delegate.
 * Register it with RegisterWithVNHost, or set AutoRegisterVNHost to register on BeginPlay.
 * The owner's transform is not saved here; the stateful actor or the save game's component pass handles the owner.
 */
UCLASS(ClassGroup = (VisualNovel), meta = (BlueprintSpawnableComponent, DisplayName = "VN Close Listener Component"), BlueprintType, Blueprintable)
class KISHIVISUALNOVEL_API URpyCloseVNEventListenerComponent : public UActorComponent, public IRpyCloseVNEventListener
{
	GENERATED_BODY()

public:
	URpyCloseVNEventListenerComponent();

	virtual void OnCloseVNEvent_Implementation() override;
	virtual TSubclassOf<URpyState> GetStateClass_Implementation() override;
	virtual URpyState *SaveToState_Implementation(UObject *Outer) override;
	virtual void LoadFromState_Implementation(const URpyState *State) override {}

	/** Registers this listener with VNHostActor, which implements IRpyMainActor or carries an IRpyMainComponent. */
	UFUNCTION(BlueprintCallable, Category = "VN Close Listener Component")
	bool RegisterWithVNHost(AActor *VNHostActor);

	/** Optional. Broadcast when the VN this listener is registered with closes. */
	UPROPERTY(BlueprintAssignable, Category = "VN Close Listener Component|Hooks")
	FOnRpyCloseVNEvent OnVNClosed;

protected:
	virtual void BeginPlay() override;

	/** Actor to register with on BeginPlay. Leave unset to register manually. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VN Close Listener Component")
	AActor *AutoRegisterVNHost = nullptr;
};
