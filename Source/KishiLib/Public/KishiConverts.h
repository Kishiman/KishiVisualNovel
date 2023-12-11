#pragma once

#include "CoreMinimal.h"

class KishiConverts
{
public:
FORCEINLINE static const float KFloat(float floa) { return floa; };
FORCEINLINE static const float KFloat(FVector vect) { return vect.Size(); };
FORCEINLINE static const float KFloat(FVector2D vect) { return vect.Size(); };
FORCEINLINE static const float KFloat(FVector4 vect) { return vect.Size(); };
FORCEINLINE static const float KFloat(FQuat quat) { return quat.GetAngle(); };
};


