#pragma once

#include "Core/Public/Containers/Array.h"

template <typename ElementType>
void SwapAll(TArray<ElementType> &array)
{
    int len = array.Num();
    for (int i = 0; i < len / 2; i++)
    {
        array.Swap(i, len - i);
    }
};