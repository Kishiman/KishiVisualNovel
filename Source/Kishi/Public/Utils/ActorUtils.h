#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ActorUtils.generated.h"

UCLASS()
class KISHI_API UActorUtils : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintPure)
  static AActor *FindActorByName(UWorld *World, FName ActorName);

  UFUNCTION(BlueprintPure)
  static void GetAllActorsWithInterfaceAndTag(UWorld *World, TSubclassOf<UInterface> Interface, const FName Tag, TArray<AActor *> &OutActors);

protected:
private:
};