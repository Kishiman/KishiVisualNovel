#pragma once

#include "CoreMinimal.h"
#include "Components/InteractionHandlerComponentBase.h"
#include "LightInteractionComponent.generated.h"

class ULightComponent;

/** Light switch: its action toggles a light and ends the interaction. */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent, DisplayName = "Light Interaction Component"), BlueprintType, Blueprintable)
class KISHIVISUALNOVEL_API ULightInteractionComponent : public UInteractionHandlerComponentBase
{
	GENERATED_BODY()

public:
	virtual void HandleInteractionStart_Implementation(FName ActionName, UInteractingComponent *InteractingComponent, UInteractableComponent *InteractableComponent) override;

	/** Sets the light to toggle. Pass None to go back to the owner's first light. */
	UFUNCTION(BlueprintCallable, Category = "Light Interaction Component")
	void SetLight(ULightComponent *InLight) { Light = InLight; }

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Light Interaction Component")
	ULightComponent *Light = nullptr;
};
