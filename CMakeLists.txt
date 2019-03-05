cmake_minimum_required(VERSION 3.0)
project(stuff)

include_directories(${CMAKE_SOURCE_DIR})
set(CXX_FLAGS "--std=c++2a -Wall -Wextra -Werror -msse4 -mavx2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CXX_FLAGS} -g -O0 -fno-omit-frame-pointer -DSTUFF_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CXX_FLAGS} -O3 -DNDEBUG")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_library(LIBFMT_LIB fmt)

# Testing
enable_testing()

add_executable(container__hash_map__test test/container/hash_map.cpp)
add_test(hash_map container__hash_map__test)
target_link_libraries(container__hash_map__test ${LIBFMT_LIB})

add_executable(container__hash_set__test test/container/hash_set.cpp)
add_test(hash_set container__hash_set__test)
target_link_libraries(container__hash_set__test ${LIBFMT_LIB})

add_executable(container__small_vector__test test/container/small_vector.cpp)
add_test(small_vector container__small_vector__test)
target_link_libraries(container__small_vector__test ${LIBFMT_LIB})

add_executable(meta__named_template_params__test test/meta/named_template_params.cpp)
add_test(meta/named_template_params meta__named_template_params__test)
target_link_libraries(meta__named_template_params__test ${LIBFMT_LIB})

add_executable(meta__nth__test test/meta/nth.cpp)
add_test(meta/nth meta__nth__test)
target_link_libraries(meta__nth__test ${LIBFMT_LIB})

add_executable(meta__range__test test/meta/range.cpp)
add_test(meta/range meta__range__test)
target_link_libraries(meta__range__test ${LIBFMT_LIB})

# Documentation
include(doc/doc.cmake)

# clang-tidy
include(cmake/tidy.cmake)
