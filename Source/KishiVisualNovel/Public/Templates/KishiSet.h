#pragma once

#include "CoreMinimal.h"

template<typename InKeyType,typename InValueType,  bool bInAllowDuplicateKeys>
struct TKishiPairKeyFuncs:public BaseKeyFuncs<TPair<InKeyType,InValueType>,InKeyType,bInAllowDuplicateKeys>{
private:
    using Super = BaseKeyFuncs<TPair<InKeyType,InValueType>,InKeyType,bInAllowDuplicateKeys>;

public:
    using KeyInitType = Super::KeyInitType;
    using ElementInitType = Super::ElementInitType;

    	/**
	 * @return The key used to index the given element.
	 */
	static FORCEINLINE KeyInitType GetSetKey(ElementInitType Element)
	{
		return Element.Key;
	}

	/**
	 * @return True if the keys match.
	 */
	template<typename ComparableKey>
	static FORCEINLINE bool Matches(KeyInitType A, ComparableKey B)
	{
		return A == B;
	}

	/** Calculates a hash index for a key. */
	static FORCEINLINE uint32 GetKeyHash(KeyInitType Key)
	{
		return GetTypeHash(Key);
	}
};

template <typename KeyType,typename ValueType, bool bInAllowDuplicateKeys>
using TKishiPairSet = TSet<TPair<KeyType,ValueType>,TKishiPairKeyFuncs<KeyType,ValueType,bInAllowDuplicateKeys>,FDefaultSetAllocator>;