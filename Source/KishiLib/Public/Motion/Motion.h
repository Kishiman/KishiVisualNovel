#pragma once

#include "CoreMinimal.h"
#include "Motion/MotionFrame.h"
#include <memory>
template <typename T>
// class IMotionable : public IInvertable<IMotionable<T>, false>

class MotionInvert;
template <typename T>
class MotionZero;
template <typename T>
class TMotionOverlay;
template <typename T>
class TMotionSequence;

template <typename T>
class IMotionable : public ThClone<IMotionable<T>, TheoryAssumtion::stand>
{
public:
  static MotionZero<T> Zero;
  IMotionable(){};
  IMotionable(const IMotionable &other) = default;
  IMotionable(IMotionable &other) = default;
  TMotionFrame<T> GetStartFrame() const { return GetMotionFrameAtNormalized(0); }
  TMotionFrame<T> GetEndFrame() const { return GetMotionFrameAtNormalized(1); };

  virtual float GetMotionTime() const { return 0; }
  virtual TMotionFrame<T> GetMotionFrameAt(float time) const { return 0; }

  TMotionFrame<T> GetMotionFrameAtNormalized(float nTime) const
  {
    if (nTime < 0 || nTime > 1)
    {
      return TMotionFrame<T>();
    }
    return GetMotionFrameAt(nTime * GetMotionTime());
  };

  MotionInvert<T> operator-() const
  {
    return MotionInvert<T>(*this);
  };

  IMotionable<T> &operator=(const IMotionable<T> &other) = default;
};
template <typename T>
TMotionOverlay<T> operator+(const IMotionable<T> &a, const IMotionable<T> &b)
{
  return TMotionOverlay<T>(a, b);
};
template <typename T>
class MotionInvert : public IMotionable<T>
{
public:
  const IMotionable<T> &origin;
  MotionInvert() = default;
  MotionInvert(MotionInvert &other) = default;
  MotionInvert(const IMotionable<T> &other) : origin(other){};

  virtual float GetMotionTime() const { return origin.GetMotionTime(); };
  virtual TMotionFrame<T> GetMotionFrameAt(float time) const
  {
    return origin.GetMotionFrameAt(origin.GetMotionTime() - time);
  };

  virtual MotionInvert<T> &operator=(const IMotionable<T> &other)
  {
    return *this;
  };
};
template <typename T>
class MotionZero : public IMotionable<T>
{
  MotionZero() = default;
  virtual float GetMotionTime() const { return 0; }
  virtual TMotionFrame<T> GetMotionFrameAt(float time) const
  {
    return TMotionFrame<T>();
  };

  virtual MotionInvert<T> &operator=(const IMotionable<T> &other)
  {
    return *this;
  };
};
