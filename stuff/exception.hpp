#pragma once

#include <exception>
#include <string>
#include <type_traits>

#include <fmt/format.h>

namespace stuff {

class exception : public std::exception {
    std::string message_;
    std::string filename_;
    std::string function_;
    int line_;

public:
    explicit exception(std::string message)
        : message_(std::move(message)),
          filename_("(unknown)"),
          function_("(unknown)"),
          line_(0) {}

    exception(std::string message, std::string filename, std::string function, int line)
        : message_(std::move(message)),
          filename_(std::move(filename)),
          function_(std::move(function)),
          line_(line) {}

    const char* what() const noexcept override { return message_.c_str(); }

    virtual const char* name() const noexcept { return "stuff::exception"; }

    const std::string& message() const noexcept { return message_; }
    const std::string& filename() const noexcept { return filename_; }
    const std::string& function() const noexcept { return function_; }
    int line() const noexcept { return line_; }
};

template <typename S>
S& operator<<(S& stream, const exception& exc) {
    return stream << "Exception (" << exc.name() << ") at " << exc.filename() << ":"
                  << exc.line() << " in " << exc.function() << ": " << exc.message();
}

namespace detail {

template <typename Exception>
struct throw_exception {
    template <typename... Args>
    [[noreturn]] void operator()(Args&&... args) {
        throw Exception(std::forward<Args>(args)...);
    }
};

}  // namespace detail

}  // namespace stuff

#define STUFF_THROW(exception_t, message)                                              \
    if constexpr (std::is_base_of_v<stuff::exception, exception_t>) {                  \
        stuff::detail::throw_exception<exception_t>{}((message), __FILE__, __func__,   \
                                                      __LINE__);                       \
    } else {                                                                           \
        stuff::detail::throw_exception<exception_t>{}(fmt::format(                     \
            "Exception in {} at {}:{}: {}", __func__, __FILE__, __LINE__, (message))); \
    }
