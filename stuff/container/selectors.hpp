#pragma once

#include <stuff/meta/named_template_params.hpp>

namespace stuff::container {

DECLARE_TYPE_SELECTOR(key);
DECLARE_TYPE_SELECTOR(value);
DECLARE_TYPE_SELECTOR(hash);
DECLARE_TYPE_SELECTOR(equal_to);
DECLARE_TYPE_SELECTOR(allocator);
DECLARE_VALUE_SELECTOR(store_hash, bool);

}  // namespace stuff::container
