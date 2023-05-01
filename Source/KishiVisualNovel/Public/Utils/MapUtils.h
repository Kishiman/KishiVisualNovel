#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

namespace MapUtils
{
  template <class K, class V>
  V FindOrMake(const TMap<K, V> &map, K key)
  {
    if (map.Contains(key))
      return map[key];
    return V();
  }

}
