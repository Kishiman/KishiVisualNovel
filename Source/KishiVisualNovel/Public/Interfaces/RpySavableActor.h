#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "RpySavableActor.generated.h"

UCLASS(BlueprintType)
class URpySavableActorState : public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ActorName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FString, FString> Properties;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Location;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator Rotation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Scale;
};

UINTERFACE(Blueprintable)
class KISHIVISUALNOVEL_API URpySavableActor : public UInterface
{
    GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpySavableActor
{
    GENERATED_BODY()

public:
    // Save state to State
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Rpy")
    URpySavableActorState *SaveToState();
    virtual URpySavableActorState *SaveToState_Implementation() = 0;

    // Load state from State
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Rpy")
    void LoadFromState(const URpySavableActorState *State);
    virtual void LoadFromState_Implementation(const URpySavableActorState *State) = 0;
};
