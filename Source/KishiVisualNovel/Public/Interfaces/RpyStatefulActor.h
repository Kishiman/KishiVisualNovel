#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Utils/ActorUtils.h"
#include "Interfaces/RpyStateful.h"
#include "Interfaces/RpyStatefulComponent.h"

#include "RpyStatefulActor.generated.h"

UCLASS(BlueprintType)
class KISHIVISUALNOVEL_API URpyStatefulActorState : public URpyState
{
    GENERATED_BODY()
public:
    virtual void PostInitProperties() override
    {
        Super::PostInitProperties();
        StateName = "RpyStatefulActorState";
    }

    virtual void PrintDebug() const override
    {
        Super::PrintDebug();
        UE_LOG(LogTemp, Warning, TEXT("ActorName: %s"), *ActorName.ToString());
        UE_LOG(LogTemp, Warning, TEXT("Location: %s"), *Location.ToString());
        UE_LOG(LogTemp, Warning, TEXT("Rotation: %s"), *Rotation.ToString());
        UE_LOG(LogTemp, Warning, TEXT("Scale: %s"), *Scale.ToString());

        for (auto &CompState : ComponentStates)
        {
            if (CompState.Value)
            {
                UE_LOG(LogTemp, Warning, TEXT("Component State: %s"), *CompState.Value->StateName.ToString());
                CompState.Value->PrintDebug();
            }
        }

        for (auto &prop : Properties)
        {
            UE_LOG(LogTemp, Warning, TEXT("Property: %s = %s"), *prop.Key, *prop.Value);
        }
    }

    UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite)
    FName ActorName;

    UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite)
    TMap<FString, FString> Properties;

    UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite)
    FVector Location;

    UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite)
    FRotator Rotation;

    UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite)
    FVector Scale;

    // NEW: All component states
    UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite)
    TMap<FName, URpyStatefulComponentState *> ComponentStates;
};

UINTERFACE(Blueprintable)
class KISHIVISUALNOVEL_API URpyStatefulActor : public URpyStateful
{
    GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyStatefulActor : public IRpyStateful
{
    GENERATED_BODY()

public:
    // get State Class
    virtual TSubclassOf<URpyState> GetStateClass_Implementation() override
    {
        return URpyStatefulActorState::StaticClass();
    }
};

UCLASS()
class KISHIVISUALNOVEL_API URpyStatefulActorLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Target"), Category = "Rpy")
    static void BaseSaveToState(const TScriptInterface<IRpyStatefulActor> &Target, URpyStatefulActorState *State)
    {
        AActor *Actor = Cast<AActor>(Target.GetObject());
        if (!Actor || !State)
            return;

        State->ActorName = Actor->GetFName();
        State->Location = Actor->GetActorLocation();
        State->Rotation = Actor->GetActorRotation();
        State->Scale = Actor->GetActorScale3D();

        State->ComponentStates.Empty();

        for (UActorComponent *Comp : Actor->GetComponents())
        {
            if (Comp && Comp->GetClass()->ImplementsInterface(URpyStatefulComponent::StaticClass()))
            {
                auto Interface = Cast<IRpyStatefulComponent>(Comp);
                if (!Interface)
                    continue;

                URpyStatefulComponentState *Saved = Cast<URpyStatefulComponentState>(Interface->Execute_SaveToState(Comp, Actor));
                if (Saved)
                {
                    FName Key = Comp->GetFName(); // stable component identifier
                    State->ComponentStates.Add(Key, Saved);
                }
            }
        }
    }

    UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Target"), Category = "Rpy")
    static void BaseLoadFromState(const TScriptInterface<IRpyStatefulActor> &Target, const URpyStatefulActorState *State)
    {
        AActor *Actor = Cast<AActor>(Target.GetObject());
        if (!Actor || !State)
            return;

        Actor->SetActorLocation(State->Location);
        Actor->SetActorRotation(State->Rotation);
        Actor->SetActorScale3D(State->Scale);

        for (auto &Pair : State->ComponentStates)
        {
            const FName ComponentName = Pair.Key;
            URpyState *CompState = Pair.Value;
            if (!CompState)
                continue;

            // Find matching component by name
            UActorComponent *Comp = UActorUtils::FindComponentByName(Actor, ComponentName);
            if (!Comp)
                continue;

            if (Comp->GetClass()->ImplementsInterface(URpyStatefulComponent::StaticClass()))
            {
                IRpyStatefulComponent::Execute_LoadFromState(Comp, CompState);
            }
        }
    }
};
