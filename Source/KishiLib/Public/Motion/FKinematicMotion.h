#pragma once

#include "CoreMinimal.h"
#include "KishiMath.h"
#include "KishiConverts.h"
#include "KishiData.h"
#include "Motion/Motion.h"
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
class TFKinematicMotion : public IMotionable<T>
{
#define S GetS()
#define U GetU()
#define V GetV()
#define A GetA()
#define URAISE                                          \
    UE_LOG(LogTemp, Error, TEXT("Unsolveable Motion")); \
    return;

private:
    TMotionFrame<T> start;
    TMotionFrame<T> end;
    float time;

public:
    //Constructors

    TFKinematicMotion() = default;
    TFKinematicMotion(const TFKinematicMotion &other)
    {
        start = other.start;
        end = other.end;
        time = other.time;
    };
    TFKinematicMotion(const TFKinematicMotion &&other)
    {
        start = other.start;
        end = other.end;
        time = other.time;
    };
    TFKinematicMotion(T s, T u, T v, T a, float t) : start(T(), u, a), end(s, v, a), time(t){};
    TFKinematicMotion(T o, T e, T u, T v, T a, float t) : start(o, u, a), end(e, v, a), time(t){};
    TFKinematicMotion(TMotionFrame<T> start, TMotionFrame<T> end, float time) : start(start), end(end), time(time){};
    TFKinematicMotion &operator=(const TFKinematicMotion &other) = default;

    //Base Getters
    virtual float GetMotionTime() const override { return time; }
    virtual TMotionFrame<T> GetMotionFrameAt(float lerp) const override
    {

        T a = start.acceleration;
        T v = start.velocity + (pow2(lerp) / 2) * start.acceleration;
        T x = start.point + (pow2(lerp) / 2) * start.velocity +
              (pow3(lerp) / 6) * start.acceleration;
        return (TMotionFrame<T>(x, v, a));
    };

    //Getters

    FORCEINLINE T GetS() { return end.point - start.point; }
    FORCEINLINE T GetU() { return start.velocity; }
    FORCEINLINE T GetV() { return end.velocity; }
    FORCEINLINE T GetA() { return start.acceleration; }
    FORCEINLINE float GetT() { return GetMotionTime(); }

    //Setters

    void Set(T o, T e, T u, T v, T a, float t)
    {
        start = TMotionFrame<T>(o, u, a);
        end = TMotionFrame<T>(e, v, a);
        time = t;
    }
    void Set(TMotionFrame<T> _start, TMotionFrame<T> _end, float _time)
    {
        this->start = _start;
        this->end = _end;
        this->time = _time;
    }
    //Protected Solvers
protected:
    /*
Kinematic Equations:
(1) S=(U+V)T/2
(2) V=U+AT
(3) S=UT+AT²/2
(4) S=VT-AT²/2
(5) V²=U²+2AS
(5') U²=V²-2AS
 */
    /*override for quaternions: +*/
    virtual void SolveDistance()
    {
        end.point = start.point + (start.velocity + end.velocity) * time / 2;
    }; //            s = (u + v) / t;

    /* override for quaternions*/
    virtual void SolveVelocity(bool InitialVelocity = true)
    {
        if (InitialVelocity)
        {
            start.velocity = end.velocity - start.acceleration * time;
        }
        else
        {
            end.velocity = start.velocity + start.acceleration * time;
        }
    }
    /* override for quaternions*/
    virtual void SolveAcceleration()
    {
        if (time != 0)
            start.acceleration = (end.velocity - start.velocity) / time;
    };
    /* override for quaternions*/
    /* override for vectors*/
    virtual void SolveTime()
    {
        if (KishiConverts::KFloat(start.velocity + end.velocity) != 0)
        {
            time = 2 * KishiConverts::KFloat(S) / KishiConverts::KFloat(start.velocity + end.velocity);
        }
        else
        {
            if (KishiConverts::KFloat(start.acceleration) != 0)
            {
                time = KishiConverts::KFloat(end.velocity - start.velocity) / KishiConverts::KFloat(start.acceleration);
            }
            else
                time = 0;
        }
    }

    //public Solvers
public:
    /*
Kinematic Equations:
(1) S=(U+V)T/2
(2) V=U+AT
(3) S=UT+AT²/2
(4) S=VT-AT²/2
(5) V²=U²+2AS
(5') U²=V²-2AS
 */
    void SolveDistanceAcceleration()
    {
        SolveDistance();
        SolveAcceleration();
    }
    /*override for quaternions */
    void SolveDistanceVelocity(bool InitialVelocity = true)
    {
        if (InitialVelocity)
        {
            end.point = start.point + end.velocity * time - (start.acceleration * time * time) / 2;
        }
        else
        {
            end.point = start.point + start.velocity * time + (start.acceleration * time * time) / 2;
        }
        SolveVelocity(InitialVelocity);
    }
    /*override for vectors */

    void SolveDistanceTime()
    {
        if (KishiConverts::KFloat(start.acceleration) != 0)
        {
            end.point = start.point + (pow2(end.velocity) - pow2(start.velocity)) / (2 * start.acceleration);
        }
        else
        {
            end.point = start.point;
        }
        SolveTime();
    }

    /*override for quaternions */

    void SolveVelocityAcceleration(bool InitialVelocity = true)
    {
        if (time != 0)
        {
            if (InitialVelocity)
            {
                start.velocity = 2 * S / time - end.velocity;
            }
            else
            {
                end.velocity = 2 * S / time - start.velocity;
            }
        }
        else
        {
            if (InitialVelocity)
                start.velocity = end.velocity;
            else
                end.velocity = start.velocity;
            return;
        }
        SolveAcceleration();
    }
    /*override for quaternions */

    void SolveVelocityBoth()
    {
        start.velocity = S / time - start.acceleration * time / 2;
        end.velocity = S / time + start.acceleration * time / 2;
    }
    /*
Kinematic Equations:
(1)   S=(U+V)T/2
(2)   V=U+AT
(3)   S=UT+AT²/2
(4)   S=VT-AT²/2
(5)   V²=U²+2AS
(5')  U²=V²-2AS
 */
    /*override for vectors */
    void SolveVelocityTime(bool InitialVelocity = true, bool maxTime = false)
    {
        float U² = pow2(V) - 2 * A * S;
        if (!InitialVelocity)
        {
            //end.velocity = 5;
            //time = 6;
            *this = -(*this);
            UE_LOG(LogTemp, Warning, TEXT("s: %f / %f /%f e: %f / %f /%f t: %f"), start.point, start.velocity, start.acceleration, end.point, end.velocity, end.acceleration, time);
            SolveVelocityTime(true, maxTime);
            UE_LOG(LogTemp, Warning, TEXT("s: %f / %f /%f e: %f / %f /%f t: %f"), start.point, start.velocity, start.acceleration, end.point, end.velocity, end.acceleration, time);
            *this = -(*this);
            UE_LOG(LogTemp, Warning, TEXT("s: %f / %f /%f e: %f / %f /%f t: %f"), start.point, start.velocity, start.acceleration, end.point, end.velocity, end.acceleration, time);

            return;
        }

        if (U² < 0)
        {
            URAISE;
        }
        start.velocity = FMath::Sqrt(U²);
        if (A == 0)
        {
            start.velocity = V;
        }
        else if (S == 0 && V == 0)
        {
            start.velocity = 0;
        }
        else if ((A <= 0 && S <= 0 && V >= 0) || (A >= 0 && S >= 0 && V <= 0))
        {
            URAISE;
        }
        if (A <= 0 && S >= 0)
        {
            //U is already positive
        }
        else if (A >= 0 && S <= 0)
        {
            start.velocity = -start.velocity;
        }
        else
        {
            start.velocity *= (maxTime ? -1 : 1) * FMath::Sign(V);
        }
        SolveTime();
    }
    /*override for vectors */
    void SolveAccelerationTime()
    {
        if (S != 0)
        {
            start.acceleration = (pow2(end.velocity) - pow2(start.velocity)) / (2 * S);
            SolveTime();
        }
        else
        {
            // throw new ArgumentException ();
        }
    }

    //Generators
    /*override */
    void Subdivide(TFKinematicMotion *TMotion1, TFKinematicMotion *TMotion2, float midTime)
    {
        TMotionFrame<T> mid = GetMotionFrameAt(midTime);
        TMotion1 = Clone(*this);
        TMotion1->Set(start, mid, midTime);
        TMotion1 = Clone(*this);
        TMotion2->Set(mid, end, time - midTime);
    }
    void SubdivideNormalized(TFKinematicMotion &TMotion1, TFKinematicMotion &TMotion2, float midTime)
    {
        Subdivide(TMotion1, TMotion2, midTime * time);
    }
    //Advanced Solvers
    /*override for vectors */
    virtual void SolveMidPointFixedTime(T midPoint, float midTime = -1)
    {
        if (midTime == -1)
            midTime = KishiConverts::KFloat(midPoint - start.point) / KishiConverts::KFloat((end.point - midPoint) + (midPoint - start.point));
        end.point = start.point + 2 * (midTime * S - midPoint) / (midTime * (1 - midTime) * pow2(this->time));
        start.velocity = (2 * S - start.acceleration * pow2(time)) / (2 * time);
    }

    void SolveMidPointFixedTimeNormalized(T midPoint, float midTime = -1)
    {
        return SolveInterpolationFixedTime(midPoint, midTime * this->time);
    };
    /*
  returns a second Motionable that plays after this motion
  override for vectors
   */
    virtual TFKinematicMotion *SolveMidPointFixedAcceleration(T midPoint, float midTime = -1)
    {
        if (midTime == -1)
            midTime = (midPoint - start.point) / ((end.point - midPoint) + (midPoint - start.point));
        if ((S) == 0)
        {
            return NULL;
        }
        time = (2 * (midTime * S - midPoint)) / (S * midTime * (1 - midTime));
        if (time <= 0)
        {
            TFKinematicMotion m1(start.point, midPoint, start.velocity, T(), T(), 0);
            m1.SolveVelocityTime();
            float t2 = m1.time * ((1 - midTime) * midTime);
            TFKinematicMotion *m2 = (new TFKinematicMotion(midPoint, end.point, T(), T(), start.acceleration, t2));
            m2->SolveVelocityBoth();
            time = m1.time;
            *this = m1;
            return m2;
        }
        time = UKismetMathLibrary::Sqrt(time);
        SolveVelocity();
        return NULL;
    }
    /*
  returns a second Motionable that plays after this motion
  override for vectors
   */
    virtual TFKinematicMotion *SolveMidPointFixedInitialVelocity(T midPoint, float midTime = -1)
    {
        if (midTime == -1)
            midTime = (midPoint - start.point) / ((end.point - midPoint) + (midPoint - start.point));
        if ((start.velocity) == 0)
            return NULL;
        time = (midPoint - midTime * midTime * S) / (midTime * (1 - midTime) * start.velocity);
        if (time < 0)
        {
            TFKinematicMotion m1(start.point, midPoint, start.velocity, T(), T(), 0);
            m1.SolveVelocityTime();
            float t2 = m1.time * ((1 - midTime) * midTime);
            TFKinematicMotion *m2 = (new TFKinematicMotion(midPoint, end.point, T(), T(), start.acceleration, t2));
            m2->SolveVelocityAcceleration(false);
            time = m1.time;
            *this = m1;
            return m2;
        }
        start.acceleration = 2 * (midTime * S - midPoint) / (midTime * (1 - midTime) * pow2(time));
        return NULL;
    }
    TFKinematicMotion operator-() const
    {
        TMotionFrame<T> _start = -this->start;
        TMotionFrame<T> _end = -this->end;
        return TFKinematicMotion(_start, _end, time);
    }

    TFKinematicMotion operator*(const float scale) const
    {
        TMotionFrame<T> _start = this->start * scale;
        TMotionFrame<T> _end = this->end * scale;
        return TFKinematicMotion(_start, _end, (time / scale));
    }

    virtual TFKinematicMotion &operator=(const IMotionable<T> &other)
    {
        start = other.GetStartFrame();
        end = other.GetEndFrame();
        time = other.GetMotionTime();
        return *this;
    };
};
#undef S
#undef U
#undef V
#undef A