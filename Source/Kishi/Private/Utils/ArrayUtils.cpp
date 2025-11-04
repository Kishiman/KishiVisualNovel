#include "Utils/ArrayUtils.h"

void UArrayUtils::CompareArrays(
    const TArray<int32> &OldArray,
    const TArray<int32> &NewArray,
    TArray<int32> &Existing,
    TArray<int32> &Removed,
    TArray<int32> &Added)
{
  // This is a dummy function so Blueprint wildcard works.
  // Actual logic is duplicated from the template version for UObject-based elements.

  Existing.Empty();
  Removed.Empty();
  Added.Empty();

  for (auto &OldItem : OldArray)
  {
    if (NewArray.Contains(OldItem))
      Existing.Add(OldItem);
    else
      Removed.Add(OldItem);
  }

  for (auto &NewItem : NewArray)
  {
    if (!OldArray.Contains(NewItem))
      Added.Add(NewItem);
  }
}

// Explicit template instantiations (optional)
template void UArrayUtils::CompareArraysTyped<int32>(
    const TArray<int32> &, const TArray<int32> &, TArray<int32> &, TArray<int32> &, TArray<int32> &);
