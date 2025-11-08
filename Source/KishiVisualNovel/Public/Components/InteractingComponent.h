#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/ActorComponent.h"

#include "Components/InteractableComponent.h"
#include "InteractingComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class KISHIVISUALNOVEL_API UInteractingComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  UInteractingComponent();

  UPROPERTY(BlueprintAssignable)
  FOnInteractionEnd OnInteractionEnd;

  UPROPERTY(BlueprintAssignable)
  FOnInteractionStart OnInteractionStart;

  UPROPERTY(BlueprintReadOnly, Category = "Interaction")
  UInteractableComponent *FocusedInteractable;

  UPROPERTY(BlueprintReadOnly, Category = "Interaction")
  TArray<UInteractableComponent *> NearbyInteractables;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  bool bScanNearbyInteractables = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  float FocusDistance = 200.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  float NearbyRadius = 150.f;

  // Call manually if needed
  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void ScanInteractableActors();

  UFUNCTION(BlueprintCallable, Category = "Interaction")
  UInteractableComponent *CallInteractOnInteractable(FName ActionName);

  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void HandleInteractionEnded();

  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void CallEndInteractionOnInteractable() const;

protected:
  // State
  UPROPERTY(BlueprintReadOnly)
  bool bIsInteracting = false;

  void SetFocusedInteractable(UInteractableComponent *NewFocusedInteractable);

  virtual void BeginPlay() override;

  // Ticks automatically
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
};

UCLASS()
class KISHIVISUALNOVEL_API UInteractableActorLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
protected:
private:
};