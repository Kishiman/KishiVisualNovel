#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ActorUtils.generated.h"

UCLASS()
class KISHI_API UActorUtils : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()

public:
  /** Finds an actor by name (exact match). Returns nullptr if not found. */
  UFUNCTION(BlueprintPure, Category = "ActorUtils", meta = (WorldContext = "WorldContextObject"))
  static AActor *FindActorByName(const UObject *WorldContextObject, FName ActorName);

  /** Gets all actors implementing an interface and optionally having a tag. */
  UFUNCTION(BlueprintCallable, Category = "ActorUtils", meta = (WorldContext = "WorldContextObject"))
  static TArray<AActor *> GetAllActorsWithInterfaceAndTag(const UObject *WorldContextObject, TSubclassOf<UInterface> Interface, FName Tag = NAME_None);

  /** Gets first actor implementing an interface (optionally filtered by tag). */
  UFUNCTION(BlueprintPure, Category = "ActorUtils", meta = (WorldContext = "WorldContextObject"))
  static AActor *FindActorWithInterfaceAndTag(const UObject *WorldContextObject, TSubclassOf<UInterface> Interface, FName Tag = NAME_None);

  UFUNCTION(BlueprintPure, Category = "ActorUtils", meta = (WorldContext = "WorldContextObject"))
  static UActorComponent *FindComponentByName(AActor *Actor, FName Name);
};
