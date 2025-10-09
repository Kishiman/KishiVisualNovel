#include "Rpy/BaseRpyGameMode.h"

#include "Interfaces/RpyStatefulActor.h"
#include "Rpy/BaseRpyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/MediaUtils.h"

UBaseRpySaveGame *ABaseRpyGameMode::CreateSaveGame()
{
    UBaseRpySaveGame *SaveGame = NewObject<UBaseRpySaveGame>();
    if (!SaveGame)
        return nullptr;

    // store level name
    SaveGame->SetLevelName_Implementation(UGameplayStatics::GetCurrentLevelName(this));

    TArray<AActor *> AllActors;
    UGameplayStatics::GetAllActorsWithInterface(GetWorld(), URpyStatefulActor::StaticClass(), AllActors);

    for (AActor *Actor : AllActors)
    {
        auto ActorState = IRpyStatefulActor::Execute_SaveToState(Actor, SaveGame);
        URpyStatefulActorState *State = Cast<URpyStatefulActorState>(ActorState);
        if (!State)
        {
            UE_LOG(LogTemp, Warning, TEXT("Actor %s returned null state"), *Actor->GetName());
            continue;
        }
        State->PrintDebug();
        SaveGame->SetStatefulActorState_Implementation(Actor, State);
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
    UGameplayStatics::GetAllActorsWithInterface(GetWorld(), URpyStatefulActor::StaticClass(), AllActors);

    for (AActor *Actor : AllActors)
    {
        auto State = SaveGame->GetStatefulActorState_Implementation(Actor);
        if (!State)
        {
            UE_LOG(LogTemp, Warning, TEXT("SaveGame returned null state for Actor %s"), *Actor->GetName());
            continue;
        }
        State->PrintDebug();
        IRpyStatefulActor::Execute_LoadFromState(Actor, State);
    }
    // if (!SaveGame->RpyState.currentInstruction.script.IsValid())
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("No valid script in save data"));
    //     return;
    // }
    // UE_LOG(LogTemp, Warning, TEXT("Restoring script %s"), *SaveGame->RpyState.currentInstruction.script.GetAssetName());
}