#pragma once

#include <cstdint>
#include <tuple>

namespace tl {

// forward
template<typename... TS>
struct type_list;

// type_list_item
template<typename Type, uint64_t index_>
struct type_list_item
{
    using type = Type;
    static constexpr const uint64_t index = index_;
};

// for_each
template<typename Func, uint64_t index, typename T, typename... TS>
void _for_each_impl(const Func& func)
{
    func(type_list_item<T, index>());
    _for_each_impl<Func, index + 1, TS...>(func);
}

template<typename Func, uint64_t index>
void _for_each_impl(const Func&)
{}

// get
template<uint64_t index, typename T, typename... TS>
struct _get_nth_type_inner;

template<typename T, typename... TS>
struct _get_nth_type_inner<0, T, TS...>
{
    using type = T;
};

template<uint64_t index, typename T, typename... TS>
struct _get_nth_type_inner
{
    using type = typename _get_nth_type_inner<index - 1, TS...>::type;
};

template<uint64_t index, typename... TS>
struct _get_nth_type
{
    using type = typename _get_nth_type_inner<index, TS...>::type;
};

template<uint64_t index>
struct _get_nth_type<index>
{};

// enumerate
template<uint64_t index, typename T, typename... TS>
struct _enumerate_impl_inner;

template<uint64_t index, typename... TS>
struct _enumerate_impl
{
    using type = typename _enumerate_impl_inner<index, TS...>::type;
};

template<uint64_t index>
struct _enumerate_impl<index>
{
    using type = type_list<>;
};

template<uint64_t index, typename T, typename... TS>
struct _enumerate_impl_inner
{
    using type =
        typename _enumerate_impl<index + 1, TS...>::type::template append<
            type_list_item<T, index>>;
};

// keep
template<typename L, uint64_t i, uint64_t... indices>
struct _keep_impl_inner;

template<typename L, uint64_t... indices>
struct _keep_impl
{
    using type = typename _keep_impl_inner<L, indices...>::type;
};

template<typename L>
struct _keep_impl<L>
{
    using type = type_list<>;
};

template<typename L, uint64_t i, uint64_t... indices>
struct _keep_impl_inner
{
    using _tail_type = typename _keep_impl<L, indices...>::type;
    using _ith_type = typename L::template get<i>;
    using type = typename _tail_type::template prepend<_ith_type>;
};

template<typename... TS>
struct type_list
{
    // Compile time operations
    template<template<typename> class W>
    using wrap = type_list<W<TS>...>;

    template<template<typename> class M>
    using map = type_list<typename M<TS>::result...>;

    template<typename T>
    using append = type_list<TS..., T>;

    template<typename T>
    using prepend = type_list<T, TS...>;

    template<uint64_t index>
    using get = typename _get_nth_type<index, TS...>::type;

    using enumerate = typename _enumerate_impl<0, TS...>::type;

    template<uint64_t... indices>
    using keep = typename _keep_impl<type_list<TS...>, indices...>::type;

    // Runtime operations
    template<typename Func>
    static void for_each(const Func& func)
    {
        _for_each_impl<Func, 0, TS...>(func);
    }

    // Conversion
    template<template<typename...> class U>
    using to = U<TS...>;

    using to_tuple = to<std::tuple>;
};
}
