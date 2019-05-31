#pragma once

#include <cstdint>
#include <tuple>

namespace typelist {

template <typename Type_, uint64_t index_>
struct TypeListItem
{
    using Type = Type_;
    static constexpr const uint64_t index = index_;
};

template <typename Func, uint64_t index, typename T, typename ...TS>
void forEachImpl(const Func& func) {
    func(TypeListItem<T, index>());
    forEachImpl<Func, index+1, TS...>(func);
}

template <typename Func, uint64_t index>
void forEachImpl(const Func&) {}

template <uint64_t index, typename T, typename ...TS>
struct GetNthType;

template <typename T, typename ...TS>
struct GetNthType<0, T, TS...>
{
    using Type = T;
};

template <uint64_t index, typename T, typename ...TS>
struct GetNthType
{
    using Type = typename GetNthType<index-1, TS...>::Type;
};

template <typename ...TS>
struct TypeList
{
    template<template <typename> class W>
    using Wrap = TypeList<W<TS>...>;

    template<template <typename> class M>
    using Map = TypeList<typename M<TS>::Result...>;

    template<template <typename ...> class U>
    using To = U<TS...>;

    template<typename Func>
    static void forEach(const Func& func) {
        forEachImpl<Func, 0, TS...>(func);
    }

    template<uint32_t index>
    using Get = typename GetNthType<index, TS...>::Type;

    using ToTuple = To<std::tuple>;
};

}
