#pragma once

namespace stuff::meta {

namespace detail {

template <typename...>
struct get_type {};

template <template <typename> typename Selector, typename DefaultType>
struct get_type<Selector<DefaultType>> {
    using type = DefaultType;
};

template <template <typename> typename Selector, typename Default, typename Arg,
          typename... Args>
struct get_type<Selector<Default>, Selector<Arg>, Args...> {
    using type = Arg;
};

template <template <typename> typename Selector, typename Default, typename Arg,
          typename... Args>
struct get_type<Selector<Default>, Arg, Args...> {
    using type = typename get_type<Selector<Default>, Args...>::type;
};

template <typename...>
struct get_value {};

template <typename T, T default_value, template <T, typename> typename Selector>
struct get_value<Selector<default_value, T>> {
    static constexpr auto value = default_value;
};

template <typename T, T default_value, template <T, typename> typename Selector,
          T given_value, typename... Args>
struct get_value<Selector<default_value, T>, Selector<given_value, T>, Args...> {
    static constexpr auto value = given_value;
};

template <typename T, T default_value, template <T, typename> typename Selector,
          typename Arg, typename... Args>
struct get_value<Selector<default_value, T>, Arg, Args...> {
    static constexpr auto value = get_value<Selector<default_value, T>, Args...>::value;
};

}  // namespace detail

template <typename... Args>
using get_t = typename detail::get_type<Args...>::type;

template <typename... Args>
inline constexpr auto get_v = detail::get_value<Args...>::value;

}  // namespace stuff::meta

#define DECLARE_TYPE_SELECTOR(name) \
    template <typename T>           \
    struct name {}

#define DECLARE_VALUE_SELECTOR(name, type)   \
    template <type value, typename T = type> \
    struct name {}
