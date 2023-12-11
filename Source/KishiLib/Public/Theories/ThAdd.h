#pragma once
#include "KishiTheory.h"
#include "Theories/ThInverse.h"

template <typename T, TheoryAssumtion TA, typename... bases>
class ThAdd : public Theory<ThAdd<T, TA, bases...>> //, ThInverse<T, TA, bases...> // virtual public bases...
{
private:
    ThAdd(){};
};

template <typename T>
class ThAdd<T, TheoryAssumtion::stand> : public Theory<ThAdd<T, TheoryAssumtion::stand>>
{
public:
    virtual ~ThAdd() = default;
    std::unique_ptr<T> Add_ptr(const std::unique_ptr<T> &other) const
    {
        return std::unique_ptr<T>(this->Add(other.operator->()));
    }
    T *Add(const T *other) const
    {
        return static_cast<T *>(this->AddImpl(other));
    }

protected:
    virtual ThAdd *AddImpl(const T *other) const
    {
        return new T((static_cast<const T &>(*this).operator+(static_cast<const T &>(*other))));
    }
};

template <typename T>
class ThAdd<T, TheoryAssumtion::promise> : public Theory<ThAdd<T, TheoryAssumtion::promise>>
{
public:
    virtual ~ThAdd() = default;
    std::unique_ptr<T> Add_ptr(const std::unique_ptr<T> &other) const
    {
        return std::unique_ptr<T>(this->Add(other.operator->()));
    }
    T *Add(const T *other) const
    {
        return static_cast<T *>(this->AddImpl(other));
    }

protected:
    virtual ThAdd *AddImpl(const T *other) const = 0;
};

template <typename T, typename base>
class ThAdd<T, TheoryAssumtion::fullfil, base> : public Theory<ThAdd<T, TheoryAssumtion::fullfil>>, public base
{
    virtual ThAdd *AddImpl(const base *other) const override
    {
        return new T((static_cast<const T &>(*this).operator+(static_cast<const T &>(*other))));
    }
};

//OPERATORS

template <typename T>
T operator-(const T &a, const T &b)
{
    return a + (-b);
};

template <typename T>
T &operator+=(T &a, const T &b)
{
    return a = a + b;
};
template <typename T>
T &operator-=(T &a, const T &b)
{
    return a = a - b;
};
