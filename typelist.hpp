#pragma once

#include <cstdint>
#include <optional>
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
        typename _enumerate_impl<index + 1, TS...>::type::template prepend<
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

template<typename Wanted, typename T>
struct _index_of_impl;

// Specialization: end of recursive if match in typelist with one list item.
template<typename Wanted>
struct _index_of_impl<Wanted, type_list<Wanted>>
{
    enum
    {
        value = 0
    };
};

// Specialization: end of recursive if no match in typelist with one list item.
template<typename Wanted, typename T>
struct _index_of_impl<Wanted, type_list<T>>
{
    enum
    {
        value = -1
    };
};

// Specialization: end of recursion on match.
template<typename Wanted, typename... Tail>
struct _index_of_impl<Wanted, type_list<Wanted, Tail...>>
{
    enum
    {
        value = 0
    };
};

// Specialization: General handling.
template<typename Wanted, typename Head, typename... Tail>
struct _index_of_impl<Wanted, type_list<Head, Tail...>>
{
private:
    enum
    {
        temp = _index_of_impl<Wanted, type_list<Tail...>>::value
    };

public:
    enum
    {
        value = temp == -1 ? -1 : 1 + temp
    };
};

template<typename... TS>
struct type_list
{
    // Compile time operations
    template<template<typename> class W>
    using wrap = type_list<W<TS>...>;

    template<template<typename> class M>
    using map = type_list<typename M<TS>::type...>;

    template<typename T>
    using append = type_list<TS..., T>;

    template<typename T>
    using prepend = type_list<T, TS...>;

    template<uint64_t index>
    using get = typename _get_nth_type<index, TS...>::type;

    template<typename T>
    static constexpr uint64_t index_of()
    {
        return _index_of_impl<T, type_list<TS...>>::value;
    }

    using enumerate = typename _enumerate_impl<0, TS...>::type;

    template<uint64_t... indices>
    using keep = typename _keep_impl<type_list<TS...>, indices...>::type;

    enum
    {
        length = sizeof...(TS)
    };

    // Runtime operations
    template<typename Func>
    static void for_each(const Func& func)
    {
        (func(TS()), ...);
    }

    template<typename Func>
    static bool for_each_until_true(const Func& func)
    {
        return (func(TS()) || ...);
    }

    template<typename T, typename Func>
    static std::optional<T> for_each_until_no_optional(const Func& func)
    {
        std::optional<T> ret(std::nullopt);
        (((ret = func(TS())) && ret.has_value()) || ...);
        return ret;
    }

    template<typename Func>
    static bool for_each_until_false(const Func& func)
    {
        return (func(TS()) && ...);
    }

    template<template<typename...> class U, typename Func>
    static auto _for_each_and_collect_helper(const Func& func)
        -> U<decltype(func(TS()))...>
    {
        return U<decltype(func(TS()))...>(func(TS())...);
    }

    template<template<typename...> class U, typename Func>
    static auto for_each_and_collect(const Func& func)
    {
        return enumerate::template _for_each_and_collect_helper<U, Func>(func);
    }

    // Conversion
    template<template<typename...> class U>
    using to = U<TS...>;

    using to_tuple = to<std::tuple>;
};
}
