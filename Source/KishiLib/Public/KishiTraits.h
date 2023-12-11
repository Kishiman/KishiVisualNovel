#pragma once
#include <memory>
//#include <bits/ptr_traits.h>

using namespace std;

class __undefined;

// Given Template<T, ...> return T, otherwise invalid.
template <typename _Tp>
struct head_tail
{
    using head = __undefined;
    using type = __undefined;
};
template <template <typename...> class _Template, typename _Tp,
          typename... _Types>
struct head_tail<_Template<_Tp, _Types...>>
{
    using head = _Tp;
    using tail = _Template<_Types...>;
};
template <typename _Tp>
using head_t = typename head_tail<_Tp>::head;
template <typename _Tp>
using tail_t = typename head_tail<_Tp>::tail;

//extract args of template as tuple
template <typename _Tp>
struct tuple_args
{
};
template <template <typename...> class _Template, typename... args>
struct tuple_args<_Template<args...>>
{
    using type = typename std::tuple<args...>::type;
};
//assign tuple args to a template

template <typename _Tp>
using tuple_args_t = typename tuple_args<_Tp>::type;

template <typename _Tp, typename _tuple>
struct assign_tuple_args
{
};
template <template <typename...> class _Template, typename... args, typename... _targs>
struct assign_tuple_args<_Template<_targs...>, std::tuple<args...>>
{
    using type = _Template<args...>;
};
template <typename _Tp, typename _tuple>
using assign_tuple_args_t = typename assign_tuple_args<_Tp, _tuple>::type;

template <typename _Tp, typename _tuple>
struct append_tuple_args_last
{
};
template <template <typename...> class _Template, typename... args, typename... _targs>
struct append_tuple_args_last<_Template<_targs...>, std::tuple<args...>>
{
    using type = _Template<_targs..., args...>;
};

//
template <typename _Tp, typename _tuple>
struct append_tuple_args_first
{
};
template <template <typename...> class _Template, typename... args, typename... _targs>
struct append_tuple_args_first<_Template<_targs...>, std::tuple<args...>>
{
    using type = _Template<args..., _targs...>;
};
template <typename>
struct PM_traits
{
};

template <class T, class U>
struct PM_traits<U T::*>
{
    using member_type = U;
};
// alliases

template <typename T>
using _type = typename T::type;
template <typename Condition>
using EnableIf = _type<std::enable_if<Condition::value>>;
template <typename Condition>
using DesableIf = _type<std::enable_if<!(Condition::value)>>;
template <typename T>
using abstract = typename std::is_abstract<T>::value;
template <typename T, typename method>
using has_member_function = std::is_member_function_pointer<decltype(&T::method)>;

template <typename _Tp, typename _tuple>
using append_tuple_args_last_t = typename append_tuple_args_last<_Tp, _tuple>::type;
template <typename _Tp, typename _tuple>
using append_tuple_args_first_t = typename append_tuple_args_first<_Tp, _tuple>::type;
template <typename _Tp, typename _Up>
using replace_first_with = append_tuple_args_first<_Up, tail_t<_Tp>>;
template <typename _Tp, typename _Up>
using replace_first_with_t = append_tuple_args_first_t<tail_t<_Tp>, _Up>;
template <typename _Tp, typename _Up>
using insert_at_first = append_tuple_args_first<_Tp, std::tuple<_Up>>;
template <typename _Tp, typename _Up>
using insert_at_first_t = append_tuple_args_first_t<_Tp, std::tuple<_Up>>;
template <typename _Tp, typename _Up>
using insert_at_last_t = append_tuple_args_last_t<_Tp, std::tuple<_Up>>;

template <typename _Tp, typename X, typename Y>
struct replace_X_with_Y
{
    using type = _Tp;
};

template <template <typename...> class _Template, typename X, typename Y, typename Z, typename... targs>
struct replace_X_with_Y<_Template<Z, targs...>, X, Y>
{
    static_assert(!(std::is_same<X, Z>::value), "X is different to Z");
    using type = insert_at_first<replace_X_with_Y<_Template<targs...>, X, Y>, Z>;
};
template <template <typename...> class _Template, typename X, typename Y, typename... targs>
struct replace_X_with_Y<_Template<X, targs...>, X, Y>
{
    using type = insert_at_first<replace_X_with_Y<_Template<targs...>, X, Y>, Y>;
};

template <typename _Tp, typename X, typename Y>
using replace_X_with_Y_t = typename replace_X_with_Y<_Tp, X, Y>::type;

template <typename _Tp>
using __make_not_void = typename conditional<is_void<_Tp>::value, __undefined, _Tp>::type;

//testing...
template <typename... T>
struct Inherit : T...
{
};
struct base4
{
    float x4;
};
template <typename... A>
struct InheritFromReplace : insert_at_last_t<tail_t<Inherit<A...>>, base4>
{
public:
    typedef head_t<Inherit<A...>> First;
    First first;
};

struct base1
{
    float x1;
};
struct base2
{
    float x2;
};
struct base3
{
    float x3;
};
struct derived : InheritFromReplace<base1, base2, base3>
{
    derived()
    {
        first.x1 = 0;
        //x1 = 0; doesn't inherit from base 1
        x2 = 0;
        x3 = 0;
        x4 = 0;
    };
};
