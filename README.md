# Introduction & requirements

`type_list` is a single-file, header-only library for managing variadic type lists in C++. The CMake
script is only for compiling the tests, to use the library, just grab typelist.hpp and place it
somewhere your compiler can find it. Requires C++17.

## Credits
This repository got forked from https://github.com/lipk/cpp-typelist
Thanks for your work!

# Features

## wrap

Wrap each type with some supplied template class `W`. Example: convert a list of simple types into
a list of `shared_ptr`s.

```cpp
using simple = type_list<int, std::vector<float>, std::string>;
using ptrs = simple::wrap<std::shared_ptr>;
```

Note: `wrap` only works with template classes with exactly 1 parameter. It might not be apparent at
first, but this excludes STL containers, as they typically have some extra, albeit defaulted
template parameters. To 'wrap' a type list into, say, an `std::vector`, use `type_list::map` and a
helper struct.

## map

`map` is the generic form of `wrap`. It takes some `M` template class as input, too, but it outputs
`M<T>::type` for each `T` element, instead of simply `M<T>`. Example: convert a list of scalar types
into a list of `std::vectors`.

```cpp
template <typename T>
struct wrap_in_vector { using type = std::vector<T>; };
using simple = type_list<double, int, float>;
using vecs = simple::map<wrap_in_vector>;
```

## append/prepend

`append` and `prepend` can be used to push an extra item into the back or the front of the list,
respectively.

## get

`get` takes an integer parameter `n` and returns the `n`th element of the list.

## enumerate

Wrap each element with `type_list_item`, which contains a `type` and the matching `index`.

## keep

Create a new list from the elements with the given indices. Example:

```cpp
using a = type_list<T0, T1, T2, T3, T4>;
using b = a::keep<0, 2, 3>;
static_assert(std::is_same<b, type_list<T0, T2, T3>>::value, "");
```

Note: in addition to filtering the list, `keep` can also be used to reorder or duplicate elements,
by supplying the appropriate indices multiple times and in different order.

## for_each/for_each_and_collect

Invoke a callable on each element. Elements are passed as `type_list_item` instances. Example: print
the size of each element in the list.

```cpp
using a = type_list<T0, T1, T2>;
a::for_each([&](auto t) {
    using T = decltype(t);
    std::cout << T::index << ' ' << sizeof(T::type) << std::endl;
});
```

`for_each_and_collect` does the same thing as `for_each`, but it also collects the result of each
function call into a container of your choice.

```cpp
using a = type_list<T0, T1, T2>;
auto tup = a::for_each_and_collect<std::tuple>([&](auto t) {
    using T = typename decltype(t)::type;
    return new T[100];
});
static_assert(std::is_same<decltype(tup), std::tuple<T0*, T1*, T2*>>::value, "");
```

`for_each_until_(true|false|no_optional)` does the same thing as `for_each`
except of breaking the loop on a specific condition / return value. Requires the function to
return either bool (for `_true|_false`) or optional.

## to/to_tuple

Ultimately, passing around a list of types is not good for much. Sooner or later, you'll want to
convert it to some instatiable type. This is what `to` is for:

```cpp
template <class ...T> struct A {};
using l = type_list<T1, T2, T3, T4>;
type_list::to<A> a;
static_assert(std::is_same<decltype(a), A<T1, T2, T3, T4>>::value, "");
```

For converting to an `std::tuple`, there is a specialized type: `to_tuple` (although
`to<std::tuple>` works fine also).
