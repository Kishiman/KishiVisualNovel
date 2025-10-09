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

    // actor states serialized data
    UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Save Data")
    TMap<FName, FRpySerializedState> ActorStates;

    // ---- IRpySaveGame interface implementation ----

    virtual TArray<uint8> GetScreenshot_Implementation() const override { return ScreenshotData; }
    virtual void SetScreenshot_Implementation(const TArray<uint8> &InScreenshot) override { ScreenshotData = (InScreenshot); }

    virtual FString GetLevelName_Implementation() const override { return LevelName; }
    virtual void SetLevelName_Implementation(const FString &InLevelName) override { LevelName = InLevelName; }
    virtual URpyStatefulActorState *GetStatefulActorState_Implementation(const AActor *Actor) const override
    {
        if (!Actor)
            return nullptr;
        const FRpySerializedState *Found = ActorStates.Find(Actor->GetFName());
        if (!Found)
            return nullptr;

        return Cast<URpyStatefulActorState>(URpyState::DeserializeStateObject(*Found, const_cast<UBaseRpySaveGame *>(this)));
    }
    virtual void SetStatefulActorState_Implementation(const AActor *Actor, const URpyStatefulActorState *State) override
    {
        if (!Actor || !State)
            return;
        FRpySerializedState Serialized;
        URpyState::SerializeStateObject(const_cast<URpyStatefulActorState *>(State), Serialized);
        ActorStates.Add(Actor->GetFName(), Serialized);
    }
};
