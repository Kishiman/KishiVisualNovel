#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ArrayUtils.generated.h"

UCLASS()
class KISHI_API UArrayUtils : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()

public:
  /**
   * Compare two arrays and return Existing, Removed, and Added elements.
   * Works with any type (Blueprint wildcard compatible).
   */
  UFUNCTION(BlueprintPure, meta = (DisplayName = "Compare Arrays", CompactNodeTitle = "COMPARE", DeterminesOutputType = "ArrayType", ArrayParm = "OldArray,NewArray,Existing,Removed,Added", ArrayTypeDependentParams = "OldArray,NewArray,Existing,Removed,Added"), Category = "Kishi|Utils|Array")
  static void CompareArrays(
      const TArray<int32> &OldArray, // dummy pin, real type handled by meta
      const TArray<int32> &NewArray,
      TArray<int32> &Existing,
      TArray<int32> &Removed,
      TArray<int32> &Added);

  // Template C++ version for typed use (non-blueprint)
  template <typename T>
  static void CompareArraysTyped(
      const TArray<T> &OldArray,
      const TArray<T> &NewArray,
      TArray<T> &Existing,
      TArray<T> &Removed,
      TArray<T> &Added)
  {
    Existing.Empty();
    Removed.Empty();
    Added.Empty();

    for (const T &Item : OldArray)
    {
      if (NewArray.Contains(Item))
        Existing.Add(Item);
      else
        Removed.Add(Item);
    }

    for (const T &Item : NewArray)
    {
      if (!OldArray.Contains(Item))
        Added.Add(Item);
    }
  }
};
