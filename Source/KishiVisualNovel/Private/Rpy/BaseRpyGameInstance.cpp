#include "Rpy/BaseRpyGameInstance.h"
#include "Rpy/BaseRpyGameMode.h"
#include "Utils/MediaUtils.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "ImageUtils.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "GameFramework/GameUserSettings.h"

void UBaseRpyGameInstance::Init()
{
    Super::Init();
    InitSaveSlots(); // Ensure all save slots are initialized
    ApplyOptions();
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UBaseRpyGameInstance::OnPostLoadMap);
}

void UBaseRpyGameInstance::OnPostLoadMap(UWorld *LoadedWorld)
{
    if (!PendingSaveToRestore)
        return;

    UE_LOG(LogTemp, Display, TEXT("Restoring save in newly loaded map..."));

    ABaseRpyGameMode *RpyGameMode = Cast<ABaseRpyGameMode>(LoadedWorld->GetAuthGameMode());
    if (RpyGameMode)
    {
        RpyGameMode->RestoreSaveGame(PendingSaveToRestore);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UBaseRpyGameInstance::OnPostLoadMap: GameMode not found"));
    }

    PendingSaveToRestore = nullptr;
}

void UBaseRpyGameInstance::Shutdown()
{
    FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
    Super::Shutdown();
}
void UBaseRpyGameInstance::ApplyOptions()
{
    // TODO: Apply the options to the game settings
}

void UBaseRpyGameInstance::LoadSaveGame(UBaseRpySaveGame *SaveGame)
{
    // Store save temporarily
    PendingSaveToRestore = SaveGame;

    FString TargetLevel = SaveGame->LevelName;
    if (TargetLevel.IsEmpty())
        TargetLevel = TEXT("DefaultLevelName");

    // Reload even if same level
    UWorld *World = GetWorld();
    FString CurrentLevel = World ? World->GetMapName() : TEXT("");

    // Always reload, even if same name
    FName LevelToOpen = FName(*TargetLevel);
    UGameplayStatics::OpenLevel(this, LevelToOpen);
}

bool UBaseRpyGameInstance::Save(int32 SlotIndex, int32 UserIndex)
{
    auto GameMode = GetWorld() ? GetWorld()->GetAuthGameMode() : nullptr;
    if (!GameMode)
        return false;
    auto RpyGameMode = Cast<ABaseRpyGameMode>(GameMode);
    if (!RpyGameMode)
        return false;
    auto SaveGame = RpyGameMode->CreateSaveGame();
    return this->SaveSlot(SaveGame, SlotIndex, UserIndex);
}

void UBaseRpyGameInstance::Load(int32 SlotIndex, int32 UserIndex)
{
    auto SaveGame = LoadSlot(SlotIndex, UserIndex);
    if (!SaveGame)
        return;
    this->LoadSaveGame(SaveGame);
}

bool UBaseRpyGameInstance::QuickSave(int32 UserIndex)
{
    auto GameMode = GetWorld() ? GetWorld()->GetAuthGameMode() : nullptr;
    if (!GameMode)
        return false;
    auto RpyGameMode = Cast<ABaseRpyGameMode>(GameMode);
    if (!RpyGameMode)
        return false;
    auto SaveGame = RpyGameMode->CreateSaveGame();
    return this->QuickSaveSlot(SaveGame, UserIndex);
}
void UBaseRpyGameInstance::QuickLoad(int32 UserIndex)
{
    auto SaveGame = this->QuickLoadSlot(UserIndex);
    if (!SaveGame)
        return;
    this->LoadSaveGame(SaveGame);
}

FString UBaseRpyGameInstance::GetSlotName(int32 SlotIndex) const
{
    return FString::Printf(TEXT("SaveSlot_%03d"), SlotIndex);
}

FString UBaseRpyGameInstance::GetQuickSaveSlotName() const
{
    return TEXT("QuickSaveSlot");
}

UBaseRpySaveGame *UBaseRpyGameInstance::GetSaveGame(int32 SlotIndex, int32 UserIndex)
{
    return Cast<UBaseRpySaveGame>(UGameplayStatics::LoadGameFromSlot(GetSlotName(SlotIndex), UserIndex));
}

bool UBaseRpyGameInstance::SaveSlot(UBaseRpySaveGame *SaveGame, int32 SlotIndex, int32 UserIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("Saving to slot %d"), SlotIndex);
    if (!SaveGame)
        return false;
    SaveGame->SlotIndex = SlotIndex;
    SaveGame->SlotName = FName(*GetSlotName(SlotIndex));
    SaveGame->isQuickSave = false;
    SaveGame->SaveDate = FDateTime::Now();
    // Save SaveGame Screenshot to a texture
    auto Future = UMediaUtils::TakeScreenshotAsync(GetWorld(), "HideTag");
    auto SlotName = SaveGame->SlotName.ToString();
    // Async wait (e.g. in another task)
    Async(EAsyncExecution::Thread, [this, Future = MoveTemp(Future), SaveGame, SlotName, SlotIndex, UserIndex]() mutable
          {
        auto PNGData = Future.Get(); // blocks until screenshot ready
        SaveGame->SetScreenshot_Implementation(PNGData);
        UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, UserIndex);
        FString Path = FPaths::ProjectSavedDir() + "Screenshots/SaveSlot_" + FString::FromInt(SlotIndex) + ".png";
        FFileHelper::SaveArrayToFile(PNGData, *Path);
        this->OnSaveGameComplete.Broadcast();

        return; });
    return true;
}

UBaseRpySaveGame *UBaseRpyGameInstance::LoadSlot(int32 SlotIndex, int32 UserIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("Loading from slot %d"), SlotIndex);
    USaveGame *Loaded = UGameplayStatics::LoadGameFromSlot(GetSlotName(SlotIndex), UserIndex);
    return Cast<UBaseRpySaveGame>(Loaded);
}

UBaseRpySaveGame *UBaseRpyGameInstance::GetQuickSaveGame(int32 UserIndex)
{
    return Cast<UBaseRpySaveGame>(UGameplayStatics::LoadGameFromSlot(GetQuickSaveSlotName(), UserIndex));
}

bool UBaseRpyGameInstance::QuickSaveSlot(UBaseRpySaveGame *SaveGame, int32 UserIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("Saving to quick save slot"));
    if (!SaveGame)
        return false;
    SaveGame->isQuickSave = true;
    SaveGame->SlotIndex = -1;
    SaveGame->SlotName = FName(*GetQuickSaveSlotName());
    SaveGame->SaveDate = FDateTime::Now();
    // Save SaveGame Screenshot to a texture
    auto Future = UMediaUtils::TakeScreenshotAsync(GetWorld(), "HideTag");
    // Async wait (e.g. in another task)
    auto SlotName = SaveGame->SlotName.ToString();
    Async(EAsyncExecution::Thread, [this, Future = MoveTemp(Future), SaveGame, SlotName, UserIndex]() mutable
          {
        auto PNGData = Future.Get(); // blocks until screenshot ready
        SaveGame->SetScreenshot_Implementation(PNGData);
        UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, UserIndex);
        FString Path = FPaths::ProjectSavedDir() + "Screenshots/QuickSaveSlot.png";
        FFileHelper::SaveArrayToFile(PNGData, *Path);
        this->OnSaveGameComplete.Broadcast();
        return; });
    return true;
}

UBaseRpySaveGame *UBaseRpyGameInstance::QuickLoadSlot(int32 UserIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("Loading from quick save slot"));
    USaveGame *Loaded = UGameplayStatics::LoadGameFromSlot(GetQuickSaveSlotName(), UserIndex);
    return Cast<UBaseRpySaveGame>(Loaded);
}

TArray<UBaseRpySaveGame *> UBaseRpyGameInstance::ListSaveSlots(int32 UserIndex)
{
    TArray<UBaseRpySaveGame *> Slots;

    for (int32 i = 0; i < MaxSaveSlots; i++)
    {
        UBaseRpySaveGame *SaveGame = LoadSlot(i, UserIndex);

        if (!SaveGame)
        {
            // Create an empty placeholder (not saved to disk yet)
            SaveGame = NewObject<UBaseRpySaveGame>();
            SaveGame->isEmpty = true;
            SaveGame->SlotIndex = i;
            SaveGame->SlotName = FName(*GetSlotName(i));
            SaveGame->LevelName = TEXT(""); // mark empty
        }

        Slots.Add(SaveGame);
    }

    return Slots;
}

void UBaseRpyGameInstance::InitSaveSlots(int32 UserIndex)
{
    for (int32 i = 0; i < MaxSaveSlots; i++)
    {
        UBaseRpySaveGame *SaveGame = LoadSlot(i, UserIndex);

        if (!SaveGame)
        {
            // Create an empty placeholder (not saved to disk yet)
            SaveGame = NewObject<UBaseRpySaveGame>();
            SaveGame->LevelName = TEXT(""); // mark empty
        }
    }
}
