// PlayerAnchorComponent.h

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Components/InteractableComponent.h"
#include "PlayerAnchorComponent.generated.h"

class UCameraComponent;

UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class KISHIVISUALNOVEL_API UPlayerAnchorComponent : public UInteractableComponent
{
  GENERATED_BODY()

public:
  UPlayerAnchorComponent();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anchor")
  FName AnchorInteractionMode = "Anchored";

  virtual void Interact(FName ActionName, UInteractingComponent *InteractingComponent) override;
  virtual void EndInteraction() override;

protected:
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
  TWeakObjectPtr<ACharacter> AnchoredPlayer;
  UCameraComponent *CameraRef;
};
