#include "Interfaces/Interactable.h"

#include "GameFramework/Actor.h"

TArray<UObject *> UInteractableLibrary::FindInteractableHandlers(AActor *Actor)
{
	TArray<UObject *> Handlers;
	if (!IsValid(Actor))
		return Handlers;
	if (Actor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
		Handlers.Add(Actor);
	for (UActorComponent *Component : Actor->GetComponents())
		if (Component && Component->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
			Handlers.Add(Component);
	return Handlers;
}

UInteractableComponent *UInteractableLibrary::FindOrAddInteractable(AActor *Actor)
{
	if (!IsValid(Actor))
		return nullptr;
	UInteractableComponent *Interactable = Actor->FindComponentByClass<UInteractableComponent>();
	if (!Interactable)
	{
		Interactable = NewObject<UInteractableComponent>(Actor);
		Interactable->SetupAttachment(Actor->GetRootComponent());
		Interactable->RegisterComponent();
	}
	return Interactable;
}
