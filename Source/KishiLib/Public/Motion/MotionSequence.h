#pragma once

#include "CoreMinimal.h"
#include "Motion/Motion.h"
#include "KishiData.h"
template <typename T>
class TMotionSequence : public IMotionable<T> // public  ThAdd<MotionInvert<T>, TheoryAssumtion::fullfil, IMotionable<T>>
{

private:
    const IMotionable<T> &first;
    const IMotionable<T> &second;

public:
    //Constructors
    TMotionSequence(const IMotionable<T> &_first, const IMotionable<T> &_second) : first(_first), second(_second){};
    TMotionSequence(TMotionSequence &other) = default;

    float GetMotionTime() const override
    {
        return first.GetMotionTime() + second.GetMotionTime();
    }
    TMotionFrame<T> GetMotionFrameAt(float time) const override
    {
        if (time < 0 || time > GetMotionTime())
            return TMotionFrame<T>();
        if (time < first.GetMotiontime())
        {
            return first.GetMotionFrameAt(time);
        }
        return second.GetMotionFrameAt(time - first.GetMotionTime());
    }

    virtual TMotionSequence &operator=(const IMotionable<T> &other)
    {
        first = other;
        second = IMotionable<T>::Zero;
        return *this;
    };
};
