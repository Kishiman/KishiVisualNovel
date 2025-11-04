#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/InputSettings.h"
#include "Engine/DataTable.h"
#include "InputCoreTypes.h"
#include "InputUtils.generated.h"

USTRUCT(BlueprintType)
struct FKeyPromptRow : public FTableRowBase
{
  GENERATED_BODY()

public:
  // The input key (Keyboard, Mouse, Gamepad, etc.)
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FKey Key;

  // Texture that represents this key visually
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UTexture2D *PromptTexture;
};

UCLASS()
class KISHI_API UInputUtils : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()

public:
  /** Returns all key mappings bound to a specific Action name (from Project Settings → Input). */
  UFUNCTION(BlueprintPure, Category = "Input|Utils")
  static TArray<FInputActionKeyMapping> GetKeysForAction(FName ActionName);

  /** Returns all key mappings bound to a specific Axis name (e.g. MoveForward, Turn). */
  UFUNCTION(BlueprintPure, Category = "Input|Utils")
  static TArray<FInputAxisKeyMapping> GetKeysForAxis(FName AxisName);

  /** Returns the first (primary) key for a given action (useful for UI display). */
  UFUNCTION(BlueprintPure, Category = "Input|Utils")
  static FKey GetPrimaryKeyForAction(FName ActionName);

  UFUNCTION(BlueprintCallable, Category = "Input|Utils")
  static UTexture2D *GetPromptForKey(FKey Key, UDataTable *PromptTable);

  /** Returns the prompt texture for the primary key bound to the given action. */
  UFUNCTION(BlueprintPure, Category = "Input|Utils")
  static UTexture2D *GetPrimaryPromptForAction(FName ActionName, UDataTable *PromptTable);
};
