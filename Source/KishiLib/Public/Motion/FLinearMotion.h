#pragma once

#include "CoreMinimal.h"
#include "KishiMath.h"
#include "KishiConverts.h"
#include "KishiData.h"
#include "Motion/Motion.h"
#include "Motion/MotionSequence.h"
#include "Motion/MotionOverlay.h"
#include <utility>

/*
Kinematic Equations:
(1) S=(U+V)T/2
(2) V=U+AT
(3) S=UT+AT²/2
(4) S=VT-AT²/2
(5) V²=U²+2AS
(5') U²=V²-2AS
 */
template <typename T>
class TFLinearMotion : public IMotionable<T>
{
    TMotionFrame<T> start;
    float time;

public:
    TFLinearMotion() = default;
    TFLinearMotion(T _point, T _velocity, float _time) : start(_point, _velocity), time(_time){};
    TFLinearMotion(TMotionFrame<T> _start, float _time) : start(_start.point, _start.velocity), time(_time){};
    TFLinearMotion(const IMotionable<T> &other) : TFLinearMotion(other.GetStartFrame(), other.GetMotionTime()){};
    TFLinearMotion(IMotionable<T> &&other) : TFLinearMotion(other){};

    TMotionFrame<T> GetMotionFrameAt(float _time) const
    {
        T e = start.point + start.velocity * time;
        return TMotionFrame<T>(e, start.velocity);
    }

    TFLinearMotion<T> operator-() const { return (*this * -1); };

    TFLinearMotion<T> operator*(float scale) const
    {
        return TFLinearMotion(start * scale, time);
    }
    TFLinearMotion<T> &operator=(const IMotionable<T> &other) override
    {
        (*this) = TFLinearMotion(other);
        return *this;
    }
};