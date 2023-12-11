#pragma once

#include "CoreMinimal.h"
#include "Theories/ThAdd.h"

template <typename T>
class TMotionFrame //public ThAdd<TMotionFrame<T>, TheoryAssumtion::fullfil>
{
private:
public:
  T point;
  T velocity;
  T acceleration;
  TMotionFrame() = default;
  TMotionFrame(const TMotionFrame &other) = default;
  TMotionFrame(T point, T velocity = T(), T acceleration = T()) : point(point), velocity(velocity), acceleration(acceleration){};

  operator TArray<T>()
  {
    T array[3] = {point, velocity, acceleration};
    return TArray<T>(array, 3);
  }
  TMotionFrame operator+(const TMotionFrame &other)
  {
    return TMotionFrame(point + other.point, velocity + other.velocity, acceleration + other.acceleration);
  }
  TMotionFrame operator-() const
  {
    return TMotionFrame(point, -velocity, acceleration);
  }
  TMotionFrame operator*(const float scale) const
  {
    return TMotionFrame(point, velocity * scale, acceleration * (scale * scale));
  }
  TMotionFrame operator/(const float scale) const
  {
    return TMotionFrame(point, velocity / scale, acceleration / (scale * scale));
  }
};
