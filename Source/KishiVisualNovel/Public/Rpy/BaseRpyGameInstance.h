#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BaseRpySaveGame.h"

#include "BaseRpyGameInstance.generated.h"

UCLASS()
class KISHIVISUALNOVEL_API UBaseRpyGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    // ---- Options (persisted separately from slots) ----
    UPROPERTY(Config, BlueprintReadWrite, Category = "Options")
    float MasterVolume = 1.0f;

    UPROPERTY(Config, BlueprintReadWrite, Category = "Options")
    float MusicVolume = 1.0f;

    UPROPERTY(Config, BlueprintReadWrite, Category = "Options")
    float SFXVolume = 1.0f;

    UPROPERTY(Config, BlueprintReadWrite, Category = "Options")
    int32 ScreenWidth = 1920;

    UPROPERTY(Config, BlueprintReadWrite, Category = "Options")
    int32 ScreenHeight = 1080;

    UPROPERTY(Config, BlueprintReadWrite, Category = "Options")
    bool bFullscreen = true;

    // ---- Runtime ----
    UPROPERTY(Transient)
    UBaseRpySaveGame *PendingSaveData;

    // ---- Configurable Save System ----
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Save")
    int32 MaxSaveSlots = 12;

    // ---- Save/Load API ----

    UFUNCTION(BlueprintCallable, Category = "Save|VN")
    bool Save(int32 SlotIndex, int32 UserIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Save|VN")
    void Load(int32 SlotIndex, int32 UserIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Save|VN")
    bool QuickSave(int32 UserIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Save|VN")
    void QuickLoad(int32 UserIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Save|VN")
    UBaseRpySaveGame *GetSaveGame(int32 SlotIndex, int32 UserIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Save|VN")
    bool SaveSlot(UBaseRpySaveGame *SaveGame, int32 SlotIndex, int32 UserIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Save|VN")
    UBaseRpySaveGame *LoadSlot(int32 SlotIndex, int32 UserIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Save|VN")
    UBaseRpySaveGame *GetQuickSaveGame(int32 UserIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Save|VN")
    bool QuickSaveSlot(UBaseRpySaveGame *SaveGame, int32 UserIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Save|VN")
    UBaseRpySaveGame *QuickLoadSlot(int32 UserIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Save|VN")
    TArray<UBaseRpySaveGame *> ListSaveSlots(int32 UserIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Save|VN")
    void ApplyOptions();

protected:
    virtual void Init() override;
    virtual void Shutdown() override;

    void InitSaveSlots(int32 UserIndex = 0);

private:
    FString GetSlotName(int32 SlotIndex) const;
    FString GetQuickSaveSlotName() const;
};
