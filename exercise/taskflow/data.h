#pragma once
template <typename ...Ts>
struct TypeList
{
    /* data */
    using type = TypeList<Ts...>;
    constexpr static size_t size = sizeof...(Ts);

    // append
    template<typename ...T>
    using append = TypeList<Ts..., T...>;
    // how to use
    // TypeList(int,double)::append<float,char>

    //exportTo
    template<template<typename ...> class T>
    using exportTo = T<Ts...>;
    // how to use
    // TypeList(int,double)::exportTo<std::tuple> -> std::tuple<int, double>
};

// some example:
template <typename T, T v>
struct integral_constant
{
    /* data */
    constexpr static T value = v;
};
// map value to type
using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;
using boolset = TypeList<true_type,false_type>;

// map 
template<typename IN,template<typename> class F>
struct Map;

template<template<typename > class F,typename ...Ts>
struct Map<TypeList<Ts...>,F>
{
    /* data */
    using type = TypeList<typename F<Ts>::type...>;
};

template<typename IN,template<typename >class F>
using Map_t =typename Map<IN,F>::type;
// how to use?
// Map_t<[1,2,3],[](a){return double(a);}>

// filter
template<template<typename >class F,typename IN, typename OUT = TypeList<> >
struct Filter
{
    using type = OUT;

};
template<template<typename > class F,typename OUT, typename H, typename ...Ts>
struct Filter<F, TypeList<H,Ts...>, OUT>:std::conditional_t<F<H>::value, Filter<TypeList<Ts...>,F,typename OUT::template append<H>>, 
                    Filter<TypeList<Ts...>,F,OUT> >{};

template<typename IN, template<typename > class F>
using Filter_t = Filter<F,IN>::type;

// fold

template<typename IN, typename INIT, template<typename, typename>class op>
struct Fold
{
    using type = INIT;
};

template<typename ACC, template<typename, typename>class OP,typename H, typename ...Ts>
struct Fold<TypeList<H,Ts...>,ACC,OP>:Fold<TypeList<Ts...>, typename OP<ACC,H>::type, OP>{};


template<typename IN, typename INIT, template<typename, typename> class OP>
using Fold_t = Fold<IN,INIT,OP>::type;

// concat 
template<typename IN1, typename IN2>
struct Concat;

template<typename ...Ts1, typename ...Ts2>
struct Concat<TypeList<Ts1...>, TypeList<Ts2...>>
{
    using type = TypeList<Ts1..., Ts2...>;
};

template<typename IN1, typename IN2>
using Concat_t = typename Concat<IN1, IN2>::type;


// ELem
template<typename IN, typename E>
struct Elem
{
    constexpr static bool value = false;
};

template<typename E,typename ...Ts>
struct Elem<TypeList<Ts...>,E>
{
    constexpr static bool value = (std::is_same_v<E,Ts>||...);
};

template<typename IN,typename E>
constexpr bool Elem_v = Elem<IN,E>::value;

//uniq
template<typename IN>
class Uniq{
    template<typename ACC, typename E>
    struct Append: std::conditional_t<Elem<ACC,E>,ACC,typename ACC::template append<E>>{};
public:
    using type = typename Fold_t<IN,TypeList<>>;
};

template<typename IN>
using Uniq_t = typename Uniq<IN>::type;