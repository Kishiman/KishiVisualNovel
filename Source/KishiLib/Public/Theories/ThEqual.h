#pragma once
#include "KishiTheory.h"

template <typename T, TheoryAssumtion TA, typename... bases>
class ThEquality : public Theory<ThEquality<T, TA, bases...>>, virtual public bases...
{
private:
    ThEquality(){};
};

//stand
template <typename T, typename base>
class ThEquality<T, TheoryAssumtion::stand, base> : public Theory<ThEquality<T, TheoryAssumtion::stand, base>>, virtual public base
{
    bool Equals(const T &other) const
    {
        return this->EqualsImpl(other);
    };
    bool operator!=(const T &other) const
    {
        return !(this->EqualsImpl(other));
    };

protected:
    virtual bool EqualsImpl(const T &other) const override
    {
        return (static_cast<const T &>(*this)) == other;
    };
};
//promise
template <typename T>
class ThEquality<T, TheoryAssumtion::promise> : public Theory<ThEquality<T, TheoryAssumtion::promise>>
{
    bool Equals(const T &other) const
    {
        return this->EqualsImpl(other);
    };
    bool operator!=(const T &other) const
    {
        return !(this->EqualsImpl(other));
    };

protected:
    virtual bool EqualsImpl(const T &one) const = 0;
};
//fullfil
template <typename T, typename base>
class ThEquality<T, TheoryAssumtion::fullfil, base> : public Theory<ThEquality<T, TheoryAssumtion::fullfil, base>>, virtual public base
{
protected:
    virtual bool EqualsImpl(const T &other) const override
    {
        return (static_cast<const T &>(*this)) == other;
    };
};

template <typename T>
bool operator==(const unique_ptr<T> &a, const unique_ptr<T> &b)
{
    return (*a) == (*b);
};

template <typename T>
bool operator!=(const unique_ptr<T> &a, const unique_ptr<T> &b)
{
    return (*a) != (*b);
};

struct MyInt : ThEquality<MyInt, TheoryAssumtion::fullfil>
{
    int value;
    bool operator==(const MyInt &other)
    {
        return value == other.value;
    }
};
