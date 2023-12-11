#pragma once

#include "CoreMinimal.h"
#include "KishiMath.h"
#include "KishiConverts.h"
#include "KishiData.h"
#include "Motion/Motion.h"
#include "Motion/MotionSequence.h"
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
static int instances;
template <typename T>
class TFStationaryMotion : public IMotionable<T> // public ThAdd<TFStationaryMotion<T>, TheoryAssumtion::fullfil, IMotionable<T>>
{
    T point;
    float time;

public:
    //static int instances;

    TFStationaryMotion(){};
    ~TFStationaryMotion(){};

    TFStationaryMotion(T _point, float _time) : point(_point), time(_time){};
    TFStationaryMotion(TMotionFrame<T> _start, float _time) : point(_start.point), time(_time){};
    TFStationaryMotion(const IMotionable<T> &other)
    {
        *this = other;
    };
    TFStationaryMotion(IMotionable<T> &&other)
    {
        *this = other;
    };
    virtual float GetMotionTime() const override
    {
        return time;
    }
    virtual TMotionFrame<T> GetMotionFrameAt(float _time) const override
    {
        return TMotionFrame<T>(point);
    }

    TFStationaryMotion<T> operator*(float scale) const
    {
        return *this;
    };
    virtual TFStationaryMotion<T> &operator=(const IMotionable<T> &other)
    {
        this->point = other.GetStartFrame().point;
        this->time = other.GetMotionTime();
        return *this;
    }
};