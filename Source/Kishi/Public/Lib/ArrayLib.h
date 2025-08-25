#pragma once
#include "CoreMinimal.h"

class ArrayLib
{
public:
  
  template <typename ElementType, typename Predicate>
  static int32 FindIndexByPredicate(const TArray<ElementType>& Array, Predicate Pred)
  {
      for (int32 Index = 0; Index < Array.Num(); ++Index)
      {
          if (Pred(Array[Index]))
          {
              return Index; // Return the first match
          }
      }
      return INDEX_NONE; // -1 if not found
  }
protected:
private:
};
