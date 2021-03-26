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
template<template<typename >class F,typename ...Ts>
struct Filter<TypeList<Ts...>,F,>
{
    /* data */

};
template<template<typename > class F,typename OUT, typename H, typename ...Ts>
struct Filter<TypeList<H,Ts...>, F, OUT>
{
    /* data */
    using type = std::conditional_t<F<H>::value, Filter<TypeList<Ts...>,F,typename OUT::template append<H>>, 
                    Filter<TypeList<Ts...>,F,OUT> >;
};
