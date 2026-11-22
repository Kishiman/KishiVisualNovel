#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Session/RpyStatefulActor.h"
#include "Session/RpySession.h"

#include "RpySaveGame.generated.h"

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API URpySaveGame : public UInterface
{
  GENERATED_BODY()
};

class KISHIVISUALNOVEL_API IRpySaveGame
{
  GENERATED_BODY()

public:
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpySaveGame|Required", meta = (ToolTip = "Required: return the screenshot stored with this save, as encoded image bytes."))
  TArray<uint8> GetScreenshot() const;
  virtual TArray<uint8> GetScreenshot_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpySaveGame::GetScreenshot")); return {}; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpySaveGame|Required", meta = (ToolTip = "Required: store Screenshot, encoded image bytes, with this save."))
  void SetScreenshot(const TArray<uint8> &Screenshot);
  virtual void SetScreenshot_Implementation(const TArray<uint8> &Screenshot) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpySaveGame::SetScreenshot")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpySaveGame|Required", meta = (ToolTip = "Required: return the name of the level this save was made in."))
  FString GetLevelName() const;
  virtual FString GetLevelName_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpySaveGame::GetLevelName")); return FString(); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpySaveGame|Required", meta = (ToolTip = "Required: store the name of the level this save is made in."))
  void SetLevelName(const FString &LevelName);
  virtual void SetLevelName_Implementation(const FString &LevelName) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpySaveGame::SetLevelName")); }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpySaveGame|Required", meta = (ToolTip = "Required: return the state saved for Actor, or None."))
  URpyStatefulActorState *GetStatefulActorState(const AActor *Actor) const;
  virtual URpyStatefulActorState *GetStatefulActorState_Implementation(const AActor *Actor) const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpySaveGame::GetStatefulActorState")); return nullptr; }

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IRpySaveGame|Required", meta = (ToolTip = "Required: store State as the saved state of Actor."))
  void SetStatefulActorState(const AActor *Actor, const URpyStatefulActorState *State);
  virtual void SetStatefulActorState_Implementation(const AActor *Actor, const URpyStatefulActorState *State) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IRpySaveGame::SetStatefulActorState")); }
};

UCLASS()
class KISHIVISUALNOVEL_API URpySaveGameLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
protected:
private:
};
