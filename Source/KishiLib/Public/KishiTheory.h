#pragma once

#include <memory>
#include "KishiData.h"
#include "KishiTraits.h"

enum TheoryAssumtion
{
  fullfil = 0,
  promise = 1,
  stand = 2,
};

template <typename T>
class Theory
{
};

template <template <typename, TheoryAssumtion, typename...> class _Theory, TheoryAssumtion _TA, typename T, typename... dbases>
class Theory<_Theory<T, _TA, dbases...>> : public Theory<T>
{
  friend _Theory<T, _TA, dbases...>;

private:
  Theory() = default;
};
