// BaseRpyGameMode.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseRpySaveGame.h"
#include "BaseRpyGameMode.generated.h"

UCLASS()
class KISHIVISUALNOVEL_API ABaseRpyGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    // called by UI or console to create a SaveGame object representing the current level
    UFUNCTION(BlueprintCallable, Category = "Save|Rpy")
    UBaseRpySaveGame *CreateSaveGame();

    // called after level is loaded to restore the state
    UFUNCTION(BlueprintCallable, Category = "Save|Rpy")
    void RestoreSaveGame(UBaseRpySaveGame *SaveData);
};
