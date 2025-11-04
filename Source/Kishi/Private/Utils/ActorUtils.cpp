#include "Utils/ActorUtils.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h" // For TActorIterator

AActor *UActorUtils::FindActorByName(const UObject *WorldContextObject, FName ActorName)
{
  if (!WorldContextObject)
    return nullptr;

  UWorld *World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
  for (TActorIterator<AActor> It(World); It; ++It)
  {
    if (It->GetFName() == ActorName)
    {
      return *It;
    }
  }
  return nullptr;
}

TArray<AActor *> UActorUtils::GetAllActorsWithInterfaceAndTag(const UObject *WorldContextObject, TSubclassOf<UInterface> Interface, FName Tag)
{
  TArray<AActor *> Result;
  if (!WorldContextObject || !*Interface)
    return Result;

  UWorld *World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
  for (TActorIterator<AActor> It(World); It; ++It)
  {
    AActor *Actor = *It;
    if (Actor->GetClass()->ImplementsInterface(Interface))
    {
      if (Tag.IsNone() || Actor->ActorHasTag(Tag))
      {
        Result.Add(Actor);
      }
    }
  }
  return Result;
}

AActor *UActorUtils::FindActorWithInterfaceAndTag(const UObject *WorldContextObject, TSubclassOf<UInterface> Interface, FName Tag)
{
  UWorld *World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
  for (TActorIterator<AActor> It(World); It; ++It)
  {
    AActor *Actor = *It;
    if (Actor->GetClass()->ImplementsInterface(Interface))
    {
      if (Tag.IsNone() || Actor->ActorHasTag(Tag))
      {
        return Actor;
      }
    }
  }
  return nullptr;
}
