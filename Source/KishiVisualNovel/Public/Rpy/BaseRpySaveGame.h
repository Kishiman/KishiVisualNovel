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
    // Screenshot
    UPROPERTY(VisibleAnywhere, Category = "Save Data")
    TArray<uint8> ScreenshotData;

    // Level name
    UPROPERTY(VisibleAnywhere, Category = "Save Data")
    FString LevelName;

    // session state
    UPROPERTY(VisibleAnywhere, Category = "Save Data")
    FRpyState SessionState;

    // ---- IRpySaveGame interface implementation ----

    virtual TArray<uint8> GetScreenshot_Implementation() const override { return ScreenshotData; }
    virtual void SetScreenshot_Implementation(const TArray<uint8> &InScreenshot) override { ScreenshotData = (InScreenshot); }

    virtual FString GetLevelName_Implementation() const override { return LevelName; }
    virtual void SetLevelName_Implementation(const FString &InLevelName) override { LevelName = InLevelName; }

    virtual FRpyState GetState_Implementation() const override { return SessionState; }
    virtual void SetState_Implementation(const FRpyState &InSessionState) override { SessionState = InSessionState; }
};
