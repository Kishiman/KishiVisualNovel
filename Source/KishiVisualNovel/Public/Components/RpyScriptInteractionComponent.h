#pragma once

#include "CoreMinimal.h"
#include "Components/InteractionHandlerComponentBase.h"
#include "RpyScriptInteractionComponent.generated.h"

class URpyCloseVNEventListenerComponent;
class URpyScript;

/**
 * Starts a VN script on the interacting actor. The interaction ends when that VN closes. With a Camera on the owner, the VN is viewed through it.
 * The interacting actor needs a VN host: IRpyMainActor, or an IRpyMainComponent such as URpyFirstPersonComponent.
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent, DisplayName = "VN Script Interaction Component"), BlueprintType, Blueprintable)
class KISHIVISUALNOVEL_API URpyScriptInteractionComponent : public UInteractionCameraHandlerComponentBase
{
	GENERATED_BODY()

public:
	virtual void HandleInteractionStart_Implementation(FName ActionName, UInteractingComponent *InteractingComponent, UInteractableComponent *InteractableComponent) override;
	virtual void HandleInteractionEnd_Implementation(UInteractingComponent *InteractingComponent, UInteractableComponent *InteractableComponent) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VN Script Interaction Component")
	URpyScript *InteractionScript = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VN Script Interaction Component")
	FName InteractionScriptLabel = TEXT("start");

private:
	UFUNCTION()
	void HandleVNClosed();

	UPROPERTY(Transient)
	URpyCloseVNEventListenerComponent *CloseListener = nullptr;
};
