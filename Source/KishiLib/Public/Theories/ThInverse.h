#pragma once
#include "KishiTheory.h"
#include "Theories/ThClone.h"

template <typename T, TheoryAssumtion TA, typename... bases>
class ThInverse : public Theory<ThInverse<T, TA, bases...>> //, ThClone<T, TA, bases...> // virtual public bases...
{
private:
    ThInverse() = 0;
};

template <typename T>
class ThInverse<T, TheoryAssumtion::stand> : public Theory<ThInverse<T, TheoryAssumtion::stand>>
{
public:
    virtual ~ThInverse() = default;
    std::unique_ptr<T> Inverse_ptr() const
    {
        return std::unique_ptr<T>(this->Inverse());
    }
    T *Inverse() const
    {
        return static_cast<T *>(this->InverseImpl());
    }

protected:
    virtual ThInverse *InverseImpl() const //override
    {
        return new T((static_cast<const T &>(*this).operator-()));
    }
};

template <typename T>
class ThInverse<T, TheoryAssumtion::promise> : public Theory<ThInverse<T, TheoryAssumtion::promise>>
{
public:
    virtual ~ThInverse() = default;
    std::unique_ptr<T> Inverse_ptr() const
    {
        return std::unique_ptr<T>(this->Inverse());
    }
    T *Inverse() const
    {
        return static_cast<T *>(this->InverseImpl());
    }

protected:
    virtual ThInverse *InverseImpl() const = 0;
};

template <typename T, typename base>
class ThInverse<T, TheoryAssumtion::fullfil, base> : public base, public Theory<ThInverse<T, TheoryAssumtion::fullfil>> // ,virtual public bases...
{
protected:
    virtual ThInverse *InverseImpl() const override
    {
        return new T((static_cast<const T &>(*this).operator-()));
    }
};

template <typename T>
unique_ptr<T> operator-(const unique_ptr<T> &object)
{
    return (-(*object)).Clone_ptr();
};
