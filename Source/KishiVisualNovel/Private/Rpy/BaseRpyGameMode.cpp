#include "Rpy/BaseRpyGameMode.h"

#include "Interfaces/RpyStatefulActor.h"
#include "Interfaces/RpyStatefulComponent.h"
#include "Rpy/BaseRpyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/MediaUtils.h"

UBaseRpySaveGame *ABaseRpyGameMode::CreateSaveGame()
{
    UBaseRpySaveGame *SaveGame = NewObject<UBaseRpySaveGame>();
    if (!SaveGame)
        return nullptr;

    // store level name
    IRpySaveGame::Execute_SetLevelName(SaveGame, UGameplayStatics::GetCurrentLevelName(this));

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
        IRpySaveGame::Execute_SetStatefulActorState(SaveGame, Actor, State);
    }

    TArray<AActor *> ActorsWithComponents;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), ActorsWithComponents);
    for (AActor *Actor : ActorsWithComponents)
    {
        if (!Actor) continue;
        // Stateful actors serialize their components as part of their actor state.
        if (Actor->GetClass()->ImplementsInterface(URpyStatefulActor::StaticClass())) continue;
        TArray<UActorComponent *> Components;
        Actor->GetComponents(Components);
        for (UActorComponent *Component : Components)
        {
            if (!Component || !Component->GetClass()->ImplementsInterface(URpyStatefulComponent::StaticClass())) continue;
            URpyState *State = IRpyStatefulComponent::Execute_SaveToState(Component, SaveGame);
            if (!State) continue;
            FRpySavedComponentState& Saved = SaveGame->StatefulComponents.AddDefaulted_GetRef();
            Saved.ActorName = Actor->GetFName();
            Saved.ComponentName = Component->GetFName();
            URpyState::SerializeStateObject(State, Saved.State);
        }
    }

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
        auto State = IRpySaveGame::Execute_GetStatefulActorState(SaveGame, Actor);
        if (!State)
        {
            UE_LOG(LogTemp, Warning, TEXT("SaveGame returned null state for Actor %s"), *Actor->GetName());
            continue;
        }
        State->PrintDebug();
        IRpyStatefulActor::Execute_LoadFromState(Actor, State);
    }

    TArray<AActor *> ActorsWithComponents;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), ActorsWithComponents);
    for (const FRpySavedComponentState& Saved : SaveGame->StatefulComponents)
    {
        AActor* ActorOwner = nullptr;
        for (AActor* CandidateActor : ActorsWithComponents)
        {
            if (CandidateActor && CandidateActor->GetFName() == Saved.ActorName)
            {
                ActorOwner = CandidateActor;
                break;
            }
        }
        if (!ActorOwner) continue;
        UActorComponent* Component = nullptr;
        TArray<UActorComponent*> Components;
        ActorOwner->GetComponents(Components);
        for (UActorComponent* Candidate : Components)
        {
            if (Candidate && Candidate->GetFName() == Saved.ComponentName)
            {
                Component = Candidate;
                break;
            }
        }
        if (!Component || !Component->GetClass()->ImplementsInterface(URpyStatefulComponent::StaticClass())) continue;
        URpyStatefulComponentState* State = Cast<URpyStatefulComponentState>(URpyState::DeserializeStateObject(Saved.State, SaveGame));
        if (State) IRpyStatefulComponent::Execute_LoadFromState(Component, State);
    }
}
