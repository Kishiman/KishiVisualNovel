#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/ActorComponent.h"
#include "Interfaces/RpyStateful.h"

#include "RpyStatefulComponent.generated.h"

//
// State Data
//
UCLASS(BlueprintType)
class KISHIVISUALNOVEL_API URpyStatefulComponentState : public URpyState
{
  GENERATED_BODY()
public:
  virtual void PostInitProperties() override
  {
    Super::PostInitProperties();
    StateName = "RpyStatefulComponentState";
  }

  virtual void PrintDebug() const override
  {
    Super::PrintDebug();
    UE_LOG(LogTemp, Warning, TEXT("ComponentName: %s"), *ComponentName.ToString());
  }

  UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite)
  FName ComponentName;
};

//
// Interface
//
UINTERFACE(Blueprintable)
class KISHIVISUALNOVEL_API URpyStatefulComponent : public URpyStateful
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyStatefulComponent : public IRpyStateful
{
  GENERATED_BODY()

public:
  virtual TSubclassOf<URpyState> GetStateClass_Implementation() override
  {
    return URpyStatefulComponentState::StaticClass();
  }
};

//
// Base Library (Blueprint-callable helper)
//
UCLASS()
class KISHIVISUALNOVEL_API URpyStatefulComponentLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Target"), Category = "Rpy")
  static void BaseSaveToState(const TScriptInterface<IRpyStatefulComponent> &Target, URpyStatefulComponentState *State)
  {
    if (!Target.GetObject())
      return;

    UActorComponent *Component = Cast<UActorComponent>(Target.GetObject());
    if (!Component)
      return;

    State->ComponentName = Component->GetFName();
  }
};
