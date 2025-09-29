#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Interfaces/RpySaveGame.h" // your interface header
#include "Rpy/RpyScript.h"

#include "BaseRpySaveGame.generated.h"

UCLASS()
class KISHIVISUALNOVEL_API UBaseRpySaveGame
    : public USaveGame,
      public IRpySaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "Save Data")
    bool isQuickSave = false;

    UPROPERTY(BlueprintReadOnly, Category = "Save Data")
    bool isEmpty = false;

    UPROPERTY(BlueprintReadOnly, Category = "Save Data")
    int32 SlotIndex;
    UPROPERTY(BlueprintReadOnly, Category = "Save Data")
    FName SlotName;
    // Screenshot
    UPROPERTY(BlueprintReadOnly, Category = "Save Data")
    TArray<uint8> ScreenshotData;

    // Level name
    UPROPERTY(BlueprintReadOnly, Category = "Save Data")
    FString LevelName;

    UPROPERTY(BlueprintReadOnly, Category = "Save Data")
    FDateTime SaveDate;

    // rpy state
    UPROPERTY(BlueprintReadOnly, Category = "Save Data")
    FRpyState RpyState;

    // actor states
    UPROPERTY(BlueprintReadOnly, Category = "Save Data")
    TMap<FName, URpySavableActorState *> ActorStates;

    // ---- IRpySaveGame interface implementation ----

    virtual TArray<uint8> GetScreenshot_Implementation() const override { return ScreenshotData; }
    virtual void SetScreenshot_Implementation(const TArray<uint8> &InScreenshot) override { ScreenshotData = (InScreenshot); }

    virtual FString GetLevelName_Implementation() const override { return LevelName; }
    virtual void SetLevelName_Implementation(const FString &InLevelName) override { LevelName = InLevelName; }

    virtual FRpyState GetRpyState_Implementation() const override { return RpyState; }
    virtual void SetRpyState_Implementation(const FRpyState &InRpyState) override { RpyState = InRpyState; }

    virtual URpySavableActorState *GetSavableActorState_Implementation(const AActor *Actor) const override
    {
        if (!Actor)
            return nullptr;
        auto FoundState = ActorStates.Find(Actor->GetFName());
        return FoundState ? *FoundState : nullptr;
    }
    virtual void SetSavableActorState_Implementation(const AActor *Actor, const URpySavableActorState *State) override
    {
        if (!Actor || !State)
            return;
        ActorStates.Add(Actor->GetFName(), DuplicateObject<URpySavableActorState>(State, this));
    }
};
