#include <type_traits>
#include <typelist.hpp>

using namespace tl;

template<typename T>
struct w
{};

#define _(...) __VA_ARGS__

#define S_CHECK_EQ(type1, type2)                                               \
    static_assert(std::is_same<type1, type2>::value, #type1 " == " #type2)

int main()
{
    using T3 = type_list<int, double, float>;
    using T0 = type_list<>;

    S_CHECK_EQ(T3::get<0>, int);
    S_CHECK_EQ(T3::get<1>, double);
    S_CHECK_EQ(T3::get<2>, float);

    S_CHECK_EQ(T3::to_tuple, _(std::tuple<int, double, float>));
    S_CHECK_EQ(T0::to_tuple, _(std::tuple<>));

    S_CHECK_EQ(T3::wrap<w>, _(type_list<w<int>, w<double>, w<float>>));
    S_CHECK_EQ(T0::wrap<w>, _(type_list<>));

    S_CHECK_EQ(T3::keep<0>, type_list<int>);
    S_CHECK_EQ(T3::keep<1>, type_list<double>);
    S_CHECK_EQ(_(T3::keep<0, 1, 2>), T3);
    S_CHECK_EQ(_(T3::keep<0, 2>), _(type_list<int, float>));
    S_CHECK_EQ(_(T3::keep<1, 0, 0>), _(type_list<double, int, int>));
    S_CHECK_EQ(T3::keep<>, type_list<>);

    S_CHECK_EQ(T3::prepend<bool>, _(type_list<bool, int, double, float>));
    S_CHECK_EQ(T3::append<bool>, _(type_list<int, double, float, bool>));
    S_CHECK_EQ(T0::prepend<bool>, _(type_list<bool>));
    S_CHECK_EQ(T0::append<bool>, _(type_list<bool>));
}
