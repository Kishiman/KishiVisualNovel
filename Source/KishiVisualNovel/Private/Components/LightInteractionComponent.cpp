#include "Components/LightInteractionComponent.h"

#include "Components/LightComponent.h"

void ULightInteractionComponent::HandleInteractionStart_Implementation(FName ActionName, UInteractingComponent *InteractingComponent, UInteractableComponent *InteractableComponent)
{
	ULightComponent *TargetLight = IsValid(Light) ? Light : GetOwner()->FindComponentByClass<ULightComponent>();
	if (TargetLight)
		TargetLight->ToggleVisibility();
	else
		UE_LOG(LogTemp, Error, TEXT("%s on %s: there is no light to toggle."), *GetName(), *GetNameSafe(GetOwner()));
	InteractableComponent->EndInteraction();
}
