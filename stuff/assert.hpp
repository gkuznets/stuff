#pragma once

#include <stuff/exception.hpp>

namespace stuff {

class assertion_error : public stuff::exception {
public:
    assertion_error(std::string message, std::string filename, std::string function,
                    int line)
        : stuff::exception(std::move(message), std::move(filename), std::move(function),
                           line) {}

    const char* name() const noexcept override { return "stuff::assertion_error"; }
};

}  // namespace stuff

#ifdef STUFF_DEBUG

#define STUFF_ASSERT(condition)                                                  \
    if (!(condition)) {                                                          \
        STUFF_THROW(stuff::assertion_error, "Assertion '" #condition "' failed") \
    }

#else  // #ifdef STUFF_DEBUG

#define STUFF_ASSERT(condition) ((void)0)

#endif  // #ifdef STUFF_DEBUG
