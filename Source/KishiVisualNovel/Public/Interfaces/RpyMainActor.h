#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Interfaces/RpySaveGame.h"     // your interface header
#include "Interfaces/RpySavableActor.h" // your interface header

#include "RpyMainActor.generated.h"

UCLASS(BlueprintType)
class KISHIVISUALNOVEL_API URpyMainActorState : public URpySavableActorState
{
    GENERATED_BODY()
public:
    // rpy state
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRpyState RpyState;
};
UINTERFACE(Blueprintable)
class KISHIVISUALNOVEL_API URpyMainActor : public URpySavableActor
{
    GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpyMainActor : public IRpySavableActor
{
    GENERATED_BODY()

public:
    // Check if VN is currently active
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Rpy")
    bool IsActive() const;
    virtual bool IsActive_Implementation() const = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Rpy")
    URpySession *GetCurrentSession() const;
    virtual URpySession *GetCurrentSession_Implementation() const = 0;

    // Start a VN script
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Rpy")
    void StartRpyScript(URpyScript *Script, FName const &Label);
    virtual void StartRpyScript_Implementation(URpyScript *Script, FName const &Label) = 0;

    // Close VN session
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Rpy")
    void CloseVN();
    virtual void CloseVN_Implementation() = 0;
};
