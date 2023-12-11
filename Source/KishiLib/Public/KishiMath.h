#pragma once

#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Core/Public/Math/UnrealMathUtility.h"

#define pow2(a) (a * a)
#define pow3(a) (a * a * a)

class KishiMath
{
public:
    /// <summary>solve a.x²+b.x+c=0</summary>
    static bool SolveQuadraulicEquation(float a, float b, float c, float &x1,float& x2)
    {
        x1 = NAN;
        x2=NAN;
        if (a == 0)
        {
            return SolveLinearEquation(b, c, x1);
        }
        float delta = (b * b - 4 * a * c);
        if (delta < 0)
            return false;
        delta = UKismetMathLibrary::Sqrt(delta);
        x1 = (-b - delta * UKismetMathLibrary::SignOfFloat(a)) / (2 * a);
        x2 = (-b + delta * UKismetMathLibrary::SignOfFloat(a)) / (2 * a);
        return true;
    }
    static bool SolveLinearEquation(float a, float b, float &x)
    {
        x = 0;
        if (a == 0)
        {
            if (b == 0)
                return true;
            return false;
        }
        x = -b / a;
        return true;
    }

    static float ModInRange(float x, float min, float max)
    {
        return Mod(x - min, max - min) + min;
    }
    static float ModInPI(float x)
    {
        return ModInRange(x, -UKismetMathLibrary::GetPI(), UKismetMathLibrary::GetPI());
    }

    static float Mod(float x, float y)
    {
        int div = UKismetMathLibrary::FFloor(x / y);
        return x - y * div;
    }
    static float ForwardAndBackward(float x, float min, float max)
    {
        return ForwardAndBackward(x - min, max - min) + min;
    }

    static float ForwardAndBackward(float x, float min, float max, bool &forward)
    {
        return ForwardAndBackward(x - min, max - min, forward) + min;
    }
    static float ForwardAndBackward(float x, float y)
    {
        bool forward;
        return ForwardAndBackward(x, y, forward);
    }
    static float ForwardAndBackward(float x, float y, bool &forward)
    {
        int div = UKismetMathLibrary::FFloor(x / y);
        if (div % 2 == 0)
        {
            forward = true;
            return x - y * div;
        }
        else
        {
            forward = false;
            return y * (div + 1) - x;
        }
    }

    static float RelativeAngleInRad(float angle)
    {
        while (angle < -UKismetMathLibrary::GetPI() || angle > UKismetMathLibrary::GetPI())
        {

            if (angle > 0)
                angle -= UKismetMathLibrary::GetPI() * 2;
            else
                angle += UKismetMathLibrary::GetPI() * 2;
        }
        return angle;
    }
    static float EulerInDeg(float angle)
    {
        while (angle < 0)
        {
            angle += 360;
        }
        return angle;
    }
    static float EulerInRad(float angle)
    {
        while (angle < 0)
        {
            angle += UKismetMathLibrary::GetPI() * 2;
        }
        return angle;
    };
};
