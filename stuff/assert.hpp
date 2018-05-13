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

class precondition_violation : public assertion_error {
public:
    using assertion_error::assertion_error;
    const char* name() const noexcept override { return "stuff::precondition_violation"; }
};

}  // namespace stuff

#ifdef STUFF_DEBUG

#define STUFF_ASSERT(condition)                                                  \
    if (!(condition)) {                                                          \
        STUFF_THROW(stuff::assertion_error, "Assertion '" #condition "' failed") \
    }

#define STUFF_CHECK_PRECONDITION(condition)                   \
    if (!(condition)) {                                       \
        STUFF_THROW(stuff::precondition_violation,            \
                    "Precondition '" #condition "' violated") \
    }

#else  // #ifdef STUFF_DEBUG

#define STUFF_ASSERT(condition) ((void)0)
#define STUFF_CHECK_PRECONDITION(condition) ((void)0)

#endif  // #ifdef STUFF_DEBUG
