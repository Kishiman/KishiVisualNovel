#include "Utils/ActorUtils.h"

#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h" // For TActorIterator

AActor *UActorUtils::FindActorByName(UWorld *World, FName ActorName)
{

  if (!World)
    return nullptr;

  FString name = ActorName.ToString();

  for (TActorIterator<AActor> It(World); It; ++It)
  {
    AActor *Actor = *It;
    if (Actor && Actor->GetName() == name)
    {
      return Actor;
    }
  }
  return nullptr; // Actor not found
}

void UActorUtils::GetAllActorsWithInterfaceAndTag(UWorld *World, TSubclassOf<UInterface> Interface, const FName Tag, TArray<AActor *> &OutActors)
{
  // Clear the output array
  OutActors.Empty();

  if (!World || !Interface)
  {
    return;
  }

  // Get all actors in the world
  TArray<AActor *> AllActors;
  UGameplayStatics::GetAllActorsWithInterface(World, Interface, AllActors);

  // Filter actors by interface and tag
  for (AActor *Actor : AllActors)
  {
    if (Actor && Actor->ActorHasTag(Tag))
    {
      OutActors.Add(Actor);
    }
  }
}
