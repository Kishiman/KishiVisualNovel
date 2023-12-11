#pragma once

#include "Runtime/Core/Public/Containers/Array.h"
#include "KishiTraits.h"

//KeyMask Template for definig abstract classes
//template <typename T>
// class ABC
// {
// };






//Clonable Template Class
template <typename T, bool non_abstract = true, typename... bases>
class IClonable : public bases...
{
public:
  virtual ~IClonable() = default;
  std::unique_ptr<T> Clone_ptr() const
  {
    return std::unique_ptr<T>(this->Clone());
  }
  T *Clone() const
  {
    return static_cast<T *>(this->CloneImpl());
  }

private:
  virtual IClonable *CloneImpl() const
  {
    return new T(static_cast<const T &>(*this));
  }
  IClonable(){};
  friend T;
  //friend First;
};

//Specialization for Abstract classes with base classes
template <typename T, typename... bases>
class IClonable<T, false, bases...> : public bases...
{
public:
  virtual ~IClonable() = default;

  std::unique_ptr<T> Clone_ptr() const
  {
    return std::unique_ptr<T>(this->Clone());
  }

  T *Clone() const
  {
    return static_cast<T *>(this->CloneImpl());
  }

private:
  virtual IClonable *CloneImpl() const = 0;
  IClonable(){};
  friend T;
};

//Specialization for non Abstract derived CRTP with base classes
template <template <typename T, bool non_abstract, typename... bases> class DERIVEDCRTP,
          typename T, typename... bases>
class IClonable<DERIVEDCRTP<T, true, bases...>, true> : public bases...
{
public:
  virtual ~IClonable() = default;
  std::unique_ptr<T> Clone_ptr() const
  {
    return std::unique_ptr<T>(this->Clone());
  }
  T *Clone() const
  {
    return static_cast<T *>(this->CloneImpl());
  }

private:
  virtual IClonable *CloneImpl() const
  {
    return new T(static_cast<const T &>(*this));
  }
  IClonable(){};
  friend DERIVEDCRTP<T, true, bases...>;
  friend T;
};
//Specialization for Abstract classes with base classes
template <template <typename T, bool non_abstract, typename... bases> class DERIVEDCRTP,
          typename T, typename... bases>
class IClonable<DERIVEDCRTP<T, false, bases...>, false> : public bases...
{
public:
  virtual ~IClonable() = default;

  std::unique_ptr<T> Clone_ptr() const
  {
    return std::unique_ptr<T>(this->Clone());
  }

  T *Clone() const
  {
    return static_cast<T *>(this->CloneImpl());
  }

private:
  virtual IClonable *CloneImpl() const = 0;
  IClonable(){};
  friend DERIVEDCRTP<T, false, bases...>;
  friend T;
};

class CloneABC : public IClonable<CloneABC, false>
{
private:
  /* data */
public:
  int x;
  int y;
  CloneABC()
  {
    x = 0;
    y = 0;
  };
  CloneABC(const CloneABC &other) { this->y = other.y; };
  ~CloneABC(){};
};

class CloneImpl1 : public IClonable<CloneImpl1, true, CloneABC>
{
private:
  /* data */
public:
  CloneImpl1()
  {
    this->x = 1;
    this->y = 1;
  };
  CloneImpl1(const CloneABC &other)
  {
    this->y = other.y;
    this->x = 1;
  };

  ~CloneImpl1(){};
};

class CloneImpl2 : public IClonable<CloneImpl2, true, CloneABC>
{
private:
  /* data */
public:
  CloneImpl2()
  {
    this->x = 2;
    this->y = 2;
  };
  CloneImpl2(const CloneABC &other)
  {
    this->x = 2;
    this->y = other.y;
  };

  ~CloneImpl2(){};
};
template <typename T>
T &Tmp(T &object)
{
  T *tmp = &object;
  return (*tmp);
}
template <typename T>
void AddToArray(TArray<T> &array, T elements, ...)
{
  va_list list;
  va_start(list, elements);
  for (int i = 0; i < array.Num(); i++)
  {
    array.Add(va_arg(list, T));
  }
  return array;
}