#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Rpy/RpySession.h"

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
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  TArray<uint8> GetScreenshot() const;
  virtual TArray<uint8> GetScreenshot_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void SetScreenshot(const TArray<uint8>& Screenshot);
  virtual void SetScreenshot_Implementation(const TArray<uint8>& Screenshot) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  FString GetLevelName() const;
  virtual FString GetLevelName_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void SetLevelName(const FString& LevelName);
  virtual void SetLevelName_Implementation(const FString& LevelName) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  FRpyState GetState() const;
  virtual FRpyState GetState_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  void SetState(const FRpyState& State);
  virtual void SetState_Implementation(const FRpyState& State) = 0;
};

UCLASS()
class KISHIVISUALNOVEL_API URpySaveGameLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
protected:
private:
};
