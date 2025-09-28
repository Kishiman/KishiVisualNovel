#include "Rpy/BaseRpyGameInstance.h"
#include "Rpy/BaseRpyGameMode.h"
#include "Utils/MediaUtils.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "ImageUtils.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "GameFramework/GameUserSettings.h"

bool UBaseRpyGameInstance::Save(int32 SlotIndex, int32 UserIndex)
{
    auto GameMode = GetWorld() ? GetWorld()->GetAuthGameMode() : nullptr;
    if (!GameMode)
        return false;
    auto RpyGameMode = Cast<ABaseRpyGameMode>(GameMode);
    if (!RpyGameMode)
        return false;
    auto GameSave = RpyGameMode->CreateSaveGame();
    return this->SaveSlot(GameSave, SlotIndex, UserIndex);
}
void UBaseRpyGameInstance::Load(int32 SlotIndex, int32 UserIndex)
{
    auto GameMode = GetWorld() ? GetWorld()->GetAuthGameMode() : nullptr;
    if (!GameMode)
        return;
    auto RpyGameMode = Cast<ABaseRpyGameMode>(GameMode);
    if (!RpyGameMode)
        return;
    auto GameSave = this->LoadSlot(SlotIndex, UserIndex);
    if (!GameSave)
        return;
    RpyGameMode->RestoreSaveGame(GameSave);
    return;
}

bool UBaseRpyGameInstance::QuickSave(int32 UserIndex)
{
    auto GameMode = GetWorld() ? GetWorld()->GetAuthGameMode() : nullptr;
    if (!GameMode)
        return false;
    auto RpyGameMode = Cast<ABaseRpyGameMode>(GameMode);
    if (!RpyGameMode)
        return false;
    auto GameSave = RpyGameMode->CreateSaveGame();
    return this->QuickSaveSlot(GameSave, UserIndex);
}
void UBaseRpyGameInstance::QuickLoad(int32 UserIndex)
{
    auto GameMode = GetWorld() ? GetWorld()->GetAuthGameMode() : nullptr;
    if (!GameMode)
        return;
    auto RpyGameMode = Cast<ABaseRpyGameMode>(GameMode);
    if (!RpyGameMode)
        return;
    auto GameSave = this->QuickLoadSlot(UserIndex);
    if (!GameSave)
        return;
    RpyGameMode->RestoreSaveGame(GameSave);
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
    auto Future = UMediaUtils::TakeScreenshotAsync(GetWorld(), "HideTag");
    auto SlotName = GetSlotName(SlotIndex);
    // Async wait (e.g. in another task)
    Async(EAsyncExecution::Thread, [Future = MoveTemp(Future), SaveGame, SlotName, SlotIndex, UserIndex]() mutable
          {
        auto PNGData = Future.Get(); // blocks until screenshot ready
        SaveGame->SetScreenshot_Implementation(PNGData);
        UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, UserIndex);
        FString Path = FPaths::ProjectSavedDir() + "Screenshots/SaveSlot_" + FString::FromInt(SlotIndex) + ".png";
        FFileHelper::SaveArrayToFile(PNGData, *Path);
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
    // Save SaveGame Screenshot to a texture
    auto Future = UMediaUtils::TakeScreenshotAsync(GetWorld(), "HideTag");
    // Async wait (e.g. in another task)
    auto SlotName = GetQuickSaveSlotName();
    Async(EAsyncExecution::Thread, [Future = MoveTemp(Future), SaveGame, SlotName, UserIndex]() mutable
          {
        auto PNGData = Future.Get(); // blocks until screenshot ready
        SaveGame->SetScreenshot_Implementation(PNGData);
        UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, UserIndex);
        FString Path = FPaths::ProjectSavedDir() + "Screenshots/QuickSaveSlot.png";
        FFileHelper::SaveArrayToFile(PNGData, *Path);
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
            SaveGame->LevelName = TEXT(""); // mark empty
        }

        Slots.Add(SaveGame);
    }

    return Slots;
}

void UBaseRpyGameInstance::ApplyOptions()
{
    // TODO: Apply the options to the game settings
}

void UBaseRpyGameInstance::Init()
{
    Super::Init();
    InitSaveSlots(); // Ensure all save slots are initialized
    ApplyOptions();
}

void UBaseRpyGameInstance::Shutdown()
{
    Super::Shutdown();
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
