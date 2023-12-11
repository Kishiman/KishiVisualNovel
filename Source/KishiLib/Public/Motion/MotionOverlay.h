#pragma once

#include "CoreMinimal.h"
#include "Motion/Motion.h"
#include "KishiData.h"
#include "KishiMath.h"

template <typename T>
class TMotionOverlay : public IMotionable<T>
{
private:
    const IMotionable<T> &first;
    const IMotionable<T> &second;

public:
    //Constructors
    TMotionOverlay(const IMotionable<T> &a, const IMotionable<T> &b) : first(a), second(b){};
    TMotionOverlay(TMotionOverlay &other) = default;

    float GetMotionTime() const //override
    {
        return FMath::Max(first.GetMotionTime(), second.GetMotionTime());
    }
    TMotionFrame<T> GetMotionFrameAt(float time) const //override
    {
        if (time < 0 || time > GetMotionTime())
            return TMotionFrame<T>();
        return first.GetMotionFrameAt(time) + second.GetMotionFrameAt(time);
    }
};
