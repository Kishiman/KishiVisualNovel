#include "Utils/InputUtils.h"

TArray<FInputActionKeyMapping> UInputUtils::GetKeysForAction(FName ActionName)
{
  TArray<FInputActionKeyMapping> Result;

  if (ActionName.IsNone())
    return Result;

  const UInputSettings *InputSettings = GetDefault<UInputSettings>();
  if (!InputSettings)
    return Result;

  const TArray<FInputActionKeyMapping> &ActionMappings = InputSettings->GetActionMappings();

  for (const FInputActionKeyMapping &Mapping : ActionMappings)
  {
    if (Mapping.ActionName == ActionName)
    {
      Result.Add(Mapping);
    }
  }

  return Result;
}

TArray<FInputAxisKeyMapping> UInputUtils::GetKeysForAxis(FName AxisName)
{
  TArray<FInputAxisKeyMapping> Result;

  if (AxisName.IsNone())
    return Result;

  const UInputSettings *InputSettings = GetDefault<UInputSettings>();
  if (!InputSettings)
    return Result;

  const TArray<FInputAxisKeyMapping> &AxisMappings = InputSettings->GetAxisMappings();

  for (const FInputAxisKeyMapping &Mapping : AxisMappings)
  {
    if (Mapping.AxisName == AxisName)
    {
      Result.Add(Mapping);
    }
  }

  return Result;
}

FKey UInputUtils::GetPrimaryKeyForAction(FName ActionName)
{
  const TArray<FInputActionKeyMapping> Keys = GetKeysForAction(ActionName);
  if (Keys.Num() > 0)
  {
    return Keys[0].Key;
  }

  return EKeys::Invalid;
}

UTexture2D *UInputUtils::GetPromptForKey(FKey Key, UDataTable *PromptTable)
{
  if (!PromptTable)
    return nullptr;

  // Iterate over table rows
  static const FString Context = TEXT("KeyPromptLookup");
  TArray<FKeyPromptRow *> Rows;
  PromptTable->GetAllRows(Context, Rows);

  for (FKeyPromptRow *Row : Rows)
  {
    if (Row && Row->Key == Key)
    {
      return Row->PromptTexture;
    }
  }

  return nullptr;
}

UTexture2D *UInputUtils::GetPrimaryPromptForAction(FName ActionName, UDataTable *PromptTable)
{
  FKey PrimaryKey = GetPrimaryKeyForAction(ActionName);
  if (PrimaryKey.IsValid())
  {
    return GetPromptForKey(PrimaryKey, PromptTable);
  }
  return nullptr;
}
