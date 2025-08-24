#include "Rpy/BaseRpyGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"

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

bool UBaseRpyGameInstance::SaveSlot(UBaseRpySaveGame *SaveData, int32 SlotIndex, int32 UserIndex)
{
    if (!SaveData)
        return false;
    return UGameplayStatics::SaveGameToSlot(SaveData, GetSlotName(SlotIndex), UserIndex);
}

UBaseRpySaveGame *UBaseRpyGameInstance::LoadSlot(int32 SlotIndex, int32 UserIndex)
{
    USaveGame *Loaded = UGameplayStatics::LoadGameFromSlot(GetSlotName(SlotIndex), UserIndex);
    return Cast<UBaseRpySaveGame>(Loaded);
}

UBaseRpySaveGame *UBaseRpyGameInstance::GetQuickSaveGame(int32 UserIndex)
{
    return Cast<UBaseRpySaveGame>(UGameplayStatics::LoadGameFromSlot(GetQuickSaveSlotName(), UserIndex));
}

bool UBaseRpyGameInstance::QuickSaveSlot(UBaseRpySaveGame *SaveData, int32 UserIndex)
{
    if (!SaveData)
        return false;
    return UGameplayStatics::SaveGameToSlot(SaveData, GetQuickSaveSlotName(), UserIndex);
}

UBaseRpySaveGame *UBaseRpyGameInstance::QuickLoadSlot(int32 UserIndex)
{
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
