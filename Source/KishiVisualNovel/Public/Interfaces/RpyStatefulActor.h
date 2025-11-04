#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Interfaces/RpyStateful.h"

#include "RpyStatefulActor.generated.h"

UCLASS(BlueprintType)
class URpyStatefulActorState : public URpyState
{
    GENERATED_BODY()
public:
    void PostInitProperties() override
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
        State->ActorName = Target.GetObject()->GetFName();
        State->Location = Cast<AActor>(Target.GetObject())->GetActorLocation();
        State->Rotation = Cast<AActor>(Target.GetObject())->GetActorRotation();
        State->Scale = Cast<AActor>(Target.GetObject())->GetActorScale3D();
    }
    UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Target"), Category = "Rpy")
    static void BaseLoadFromState(const TScriptInterface<IRpyStatefulActor> &Target, URpyStatefulActorState *State)
    {
        // Target.GetObject()->SetFName(State->ActorName);
        Cast<AActor>(Target.GetObject())->SetActorLocation(State->Location);
        Cast<AActor>(Target.GetObject())->SetActorRotation(State->Rotation);
        Cast<AActor>(Target.GetObject())->SetActorScale3D(State->Scale);
    }

protected:
private:
};
