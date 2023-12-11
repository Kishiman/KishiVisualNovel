#pragma once
#include "KishiTheory.h"

template <typename T, TheoryAssumtion TA, typename... bases>
class ThClone : public Theory<ThClone<T, TA, bases...>>, virtual public bases...
{
private:
    ThClone() {};
};

//stand
template <typename T>
class ThClone<T, TheoryAssumtion::stand> : public Theory<ThClone<T, TheoryAssumtion::stand>>
{
public:
    virtual ~ThClone() = default;
    std::unique_ptr<T> Clone_ptr() const
    {
        return std::unique_ptr<T>(this->Clone());
    }
    T *Clone() const
    {
        return static_cast<T *>(this->CloneImpl());
    }

protected:
    virtual ThClone *CloneImpl() const
    {
        return static_cast<ThClone *>(new T(static_cast<const T &>(*this)));
    }
};

//stand
template <typename T>
class ThClone<T, TheoryAssumtion::promise> : public Theory<ThClone<T, TheoryAssumtion::promise>>
{
public:
    virtual ~ThClone() = default;
    std::unique_ptr<T> Clone_ptr() const
    {
        return std::unique_ptr<T>(this->Clone());
    }
    T *Clone() const
    {
        return static_cast<T *>(this->CloneImpl());
    }

protected:
    virtual ThClone *CloneImpl() const = 0;
};

//fullfil
template <typename T, typename base>
class ThClone<T, TheoryAssumtion::fullfil, base> : public Theory<ThClone<T, TheoryAssumtion::fullfil>>, virtual public base
{
protected:
    virtual ThClone *CloneImpl() const
    {
        return static_cast<ThClone *>(new T(static_cast<const T &>(*this)));
    }
};

template <typename T>
unique_ptr<T> Clone(const unique_ptr<T> &object)
{
    return unique_ptr<T>((new T(object)));
};
template <typename T>
T Clone(T &object)
{
    return T(object);
};
