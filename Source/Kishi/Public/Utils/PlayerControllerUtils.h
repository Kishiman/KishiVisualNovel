#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlayerControllerUtils.generated.h"

UENUM(BlueprintType)
enum class EPlayerInputMode : uint8
{
  Gameplay UMETA(DisplayName = "Gameplay"),
  GameAndUI UMETA(DisplayName = "Game + UI"),
  UIOnly UMETA(DisplayName = "UI Only")
};

UCLASS()
class KISHI_API UPlayerControllerUtils : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()

public:
  UFUNCTION(BlueprintCallable, Category = "Input Mode")
  static void SetPlayerInputMode(APlayerController *PC, EPlayerInputMode NewMode);

  UFUNCTION(BlueprintCallable, Category = "Input Mode")
  static void RestoreDefaultInputMode(APlayerController *PC);
};
