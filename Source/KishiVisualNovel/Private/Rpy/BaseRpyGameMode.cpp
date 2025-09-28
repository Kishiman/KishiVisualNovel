#include "Rpy/BaseRpyGameMode.h"

#include "Interfaces/RpySavableActor.h"
#include "Rpy/BaseRpyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/MediaUtils.h"

UBaseRpySaveGame *ABaseRpyGameMode::CreateSaveGame()
{
    UBaseRpySaveGame *SaveGame = NewObject<UBaseRpySaveGame>();
    if (!SaveGame)
        return nullptr;

    SaveGame->SaveDate = FDateTime::UtcNow();

    // store level name
    SaveGame->SetLevelName_Implementation(UGameplayStatics::GetCurrentLevelName(this));

    TArray<AActor *> AllActors;
    UGameplayStatics::GetAllActorsWithInterface(GetWorld(), URpySavableActor::StaticClass(), AllActors);

    for (AActor *Actor : AllActors)
    {
        auto State = IRpySavableActor::Execute_SaveToState(Actor);
        if (!State)
            continue;
        SaveGame->SetSavableActorState_Implementation(Actor, State);
    }

    // TODO: store your VN state here
    // Save->SetScript(CurrentScriptPtr);
    // Save->SetScriptInstructionIndex(CurrentLineIndex);
    // Save->SetRuntimeData(CurrentDynamicState);

    // take screenshot if you want thumbnails
    // SaveGame->SetScreenshot( CaptureScreenshotBytes() );

    return SaveGame;
}

void ABaseRpyGameMode::RestoreSaveGame(UBaseRpySaveGame *SaveGame)
{
    if (!SaveGame)
        return;

    TArray<AActor *> AllActors;
    UGameplayStatics::GetAllActorsWithInterface(GetWorld(), URpySavableActor::StaticClass(), AllActors);

    for (AActor *Actor : AllActors)
    {
        auto State = SaveGame->GetSavableActorState_Implementation(Actor);
        if (!State)
            continue;
        IRpySavableActor::Execute_LoadFromState(Actor, State);
    }
    if (!SaveGame->RpyState.currentInstruction.script.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("No valid script in save data"));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("Restoring script %s"), *SaveGame->RpyState.currentInstruction.script.GetAssetName());

    // load the script and jump to the stored position
    // CurrentScriptPtr = SaveGame->GetScript_Implementation();
    // CurrentLineIndex = SaveGame->GetScriptInstructionIndex_Implementation();
    // CurrentDynamicState = SaveGame->GetRuntimeData_Implementation();
    // ...apply whatever else is needed to bring the scene back
}