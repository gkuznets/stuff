#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <memory>

#include <fmt/format.h>
#include <iostream>
#include <string>

namespace stuff::container {

namespace detail {

template <typename It>
void shift_left(It begin, It end) {
    while (begin != end) {
        *begin = *std::next(begin);
        ++begin;
    }
}

template <typename It>
void shift_right(It begin, It end) {
    while (end != begin) {
        *end = *std::prev(end);
        --end;
    }
}

template <class InputIt, class OutputIt>
void copy_all_but_one(InputIt first, InputIt last, unsigned omit_position,
                      OutputIt result) {
    for (unsigned position = 0; first != last; ++position, ++first) {
        if (position != omit_position) {
            *result++ = *first;
        }
    }
}

}  // namespace detail

template <typename Value>
class adaptive_radix_tree {
    using Key = int;
    static constexpr unsigned key_length = sizeof(Key);

    struct node_header {
        uint8_t children_count = 0;  // add 1
        uint8_t path_size = 0;
        std::array<std::uint8_t, 6> path;
    };

    union payload {
        node_header* node;
        Value* value;

        bool is_null(unsigned /* depth */) const noexcept {
            return node == nullptr;  // FIXME: use depth
        }
    };  // TODO: think of a better name

    struct node4 : node_header {
        std::array<std::uint8_t, 4> keys;
        std::array<payload, 4> children;
    };

    struct node16 : node_header {
        std::array<std::uint8_t, 16> keys;
        std::array<payload, 16> children;
    };

    struct node48 : node_header {
        static constexpr std::uint8_t none_index = 255;

        std::array<std::uint8_t, 256> child_index;
        std::array<payload, 48> children;

        node48() noexcept { child_index.fill(none_index); }
    };

    struct node256 : node_header {
        std::array<payload, 256> children;

        node256() noexcept { memset(children.data(), 0, sizeof(children)); }
    };

    static std::unique_ptr<node4> create_node4() { return std::make_unique<node4>(); }
    static std::unique_ptr<node16> create_node16() { return std::make_unique<node16>(); }
    static std::unique_ptr<node48> create_node48() { return std::make_unique<node48>(); }
    static std::unique_ptr<node256> create_node256() {
        return std::make_unique<node256>();
    }

    static void destroy(node_header* node, unsigned depth) noexcept {
        if (node) {
            if (node->children_count <= 4) {
                destroy(static_cast<node4*>(node), depth);
            } else if (node->children_count <= 16) {
                destroy(static_cast<node16*>(node), depth);
            } else if (node->children_count <= 48) {
                destroy(static_cast<node48*>(node), depth);
            } else {
                destroy(static_cast<node256*>(node), depth);
            }
        }
    }

    template <std::size_t size>
    static void destroy(std::array<payload, size>& children, unsigned children_count,
                        unsigned depth) noexcept {
        for (unsigned c = 0; c < children_count; ++c) {
            if (depth < sizeof(Key) - 1) {
                destroy(children[c].node, depth + 1);
            } else {
                delete children[c].value;
            }
        }
    }

    static void destroy(node4* node, unsigned depth) noexcept {
        destroy(node->children, node->children_count, depth);
        delete node;
    }

    static void destroy(node16* node, unsigned depth) noexcept {
        destroy(node->children, node->children_count, depth);
        delete node;
    }

    static void destroy(node48* node, unsigned depth) noexcept {
        destroy(node->children, node->children_count, depth);
        delete node;
    }

    static void destroy(node256* node, unsigned depth) noexcept {
        for (auto& child : node->children) {
            if (depth < sizeof(Key) - 1) {
                destroy(child.node, depth + 1);
            } else {
                delete child.value;
            }
        }
        delete node;
    }

    node_header* root_ = nullptr;

    bool insert(Key key, const Value& value, node_header** node,
                unsigned key_length_in_bytes) {
        const auto key_part = static_cast<std::uint8_t>(key);
        payload* following_payload = nullptr;
        if (!*node) {
            auto new_node = create_node4();
            new_node->keys[0] = key_part;
            following_payload = &new_node->children[0];
            *node = new_node.get();
            new_node.release();
        } else {
            const auto children_count = (*node)->children_count + 1;
            if (children_count <= 4) {
                auto node_4 = static_cast<node4*>(*node);
                auto& keys = node_4->keys;
                auto& children = node_4->children;
                auto pos = std::lower_bound(keys.begin(), keys.begin() + children_count,
                                            key_part);
                if (pos != keys.begin() + children_count && *pos == key_part) {
                    following_payload = &children[pos - keys.begin()];
                } else {
                    // insert new entry
                    if (children_count < 4) {
                        detail::shift_right(pos, keys.begin() + children_count);
                        detail::shift_right(children.begin() + (pos - keys.begin()),
                                            children.begin() + children_count);
                        *pos = key_part;
                        children[pos - keys.begin()].node =
                            nullptr;  // TODO: fix this access
                        following_payload = &children[pos - keys.begin()];
                        node_4->children_count++;
                    } else {
                        auto new_node = create_node16();
                        new_node->children_count = node_4->children_count + 1;
                        unsigned c = 0;
                        for (; c < 4 && keys[c] < key_part; ++c) {
                            new_node->keys[c] = keys[c];
                            new_node->children[c] = children[c];
                        }
                        new_node->keys[c] = key_part;
                        following_payload = &new_node->children[c];
                        for (; c < 4; ++c) {
                            new_node->keys[c + 1] = keys[c];
                            new_node->children[c + 1] = children[c];
                        }
                        *node = new_node.get();
                        new_node.release();
                        delete node_4;
                    }
                }
            } else if (children_count <= 16) {
                auto node_16 = static_cast<node16*>(*node);
                auto& keys = node_16->keys;
                auto& children = node_16->children;
                auto pos = std::lower_bound(keys.begin(), keys.begin() + children_count,
                                            key_part);
                if (*pos == key_part) {
                    following_payload = &children[pos - keys.begin()];
                } else {
                    // insert new entry
                    if (children_count < 16) {
                        detail::shift_right(pos, keys.begin() + children_count);
                        detail::shift_right(children.begin() + (pos - keys.begin()),
                                            children.begin() + children_count);
                        *pos = key_part;
                        children[pos - keys.begin()].node =
                            nullptr;  // TODO: fix this access
                        following_payload = &children[pos - keys.begin()];
                        node_16->children_count++;
                    } else {
                        auto new_node = create_node48();
                        new_node->children_count = node_16->children_count + 1;
                        std::uint8_t c = 0;
                        for (; c < 16 && keys[c] < key_part; ++c) {
                            new_node->child_index[keys[c]] = c;
                            new_node->children[c] = children[c];
                        }
                        new_node->child_index[key_part] = c;
                        new_node->children[c].node = nullptr;  // TODO: fix this
                        following_payload = &new_node->children[c];
                        for (; c < 16; ++c) {
                            new_node->child_index[keys[c]] = c + 1;
                            new_node->children[c + 1] = children[c];
                        }
                        *node = new_node.get();
                        new_node.release();
                        delete node_16;
                    }
                }
            } else if (children_count <= 48) {
                auto node_48 = static_cast<node48*>(*node);
                auto& child_index = node_48->child_index;
                auto& children = node_48->children;
                if (child_index[key_part] != node48::none_index) {
                    following_payload = &children[child_index[key_part]];
                } else {
                    if (children_count < 48) {
                        child_index[key_part] = children_count;
                        children[children_count].node = nullptr;  // FIXME
                        following_payload = &children[children_count];
                        node_48->children_count++;
                    } else {
                        auto new_node = create_node256();
                        new_node->children_count = node_48->children_count + 1;
                        for (int idx = 0; idx < 256; ++idx) {
                            if (child_index[idx] != node48::none_index) {
                                new_node->children[idx] = children[child_index[idx]];
                            }
                        }
                        new_node->children[key_part].node = nullptr;  // TODO: fix this
                        following_payload = &new_node->children[key_part];
                        *node = new_node.get();
                        new_node.release();
                        delete node_48;
                    }
                }
            } else {
                auto node_256 = static_cast<node256*>(*node);
                following_payload = &node_256->children[key_part];
                if (!node_256->children[key_part].node) {  // TODO: fix this access
                    node_256->children_count++;
                }
            }
        }
        if (key_length_in_bytes > 1) {
            return insert(key >> 8, value, &following_payload->node,
                          key_length_in_bytes - 1);
        } else {
            if (following_payload->value) {
                return false;
            }
            following_payload->value = new Value(value);
            return true;
        }
    }

    static const Value* find(const Key& key, const node_header& node,
                             unsigned depth) noexcept {
        const auto key_part = static_cast<std::uint8_t>(key);
        const unsigned children_count = node.children_count + 1;
        const payload* next_payload = nullptr;
        if (children_count <= 4) {
            const auto& node_4 = static_cast<const node4&>(node);
            for (unsigned c = 0; c < children_count; ++c) {
                if (node_4.keys[c] == key_part) {
                    next_payload = &node_4.children[c];
                    break;
                }
            }
        } else if (children_count <= 16) {
            //  TODO: replace with binary search
            const auto& node_16 = static_cast<const node16&>(node);
            for (unsigned c = 0; c < children_count; ++c) {
                if (node_16.keys[c] == key_part) {
                    next_payload = &node_16.children[c];
                    break;
                }
            }
        } else if (children_count <= 48) {
            const auto& node_48 = static_cast<const node48&>(node);
            if (node_48.child_index[key_part] != node48::none_index) {
                next_payload = &node_48.children[node_48.child_index[key_part]];
            }
        } else {
            const auto& node_256 = static_cast<const node256&>(node);
            if (!node_256.children[key_part].is_null(depth)) {  // TODO: fix this access
                next_payload = &node_256.children[key_part];
            }
        }

        if (!next_payload) {
            return nullptr;
        }
        if (key_length - depth > 1) {
            return find(key >> 8, *next_payload->node, depth + 1);
        } else {
            return next_payload->value;
        }
    }

    static bool erase(const Key& key, node_header** node, unsigned key_length_in_bytes) {
        if (!node) {
            return false;
        }
        const auto key_part = static_cast<std::uint8_t>(key);
        const unsigned children_count = (*node)->children_count + 1;
        if (children_count <= 4) {
            auto node_4 = static_cast<node4*>(*node);
            auto& keys = node_4->keys;
            auto& children = node_4->children;
            for (unsigned c = 0; c < children_count; ++c) {
                if (keys[c] != key_part) {
                    continue;
                }
                if (key_length_in_bytes > 1) {
                    if (!erase(key >> 8, &children[c].node, key_length_in_bytes - 1)) {
                        return false;
                    }
                    // Child node was not emptied.
                    if (children[c].node) {
                        return true;
                    }
                } else {
                    delete children[c].value;
                }
                if (children_count == 1) {
                    delete node_4;
                    *node = nullptr;
                } else {
                    node_4->children_count--;
                    detail::shift_left(keys.begin() + c, keys.begin() + children_count);
                    detail::shift_left(children.begin() + c,
                                       children.begin() + children_count);
                }
                return true;
            }
        } else if (children_count <= 16) {
            auto node_16 = static_cast<node16*>(*node);
            auto& keys = node_16->keys;
            auto& children = node_16->children;
            for (unsigned c = 0; c < children_count; ++c) {
                if (keys[c] != key_part) {
                    continue;
                }
                if (key_length_in_bytes > 1) {
                    if (!erase(key >> 8, &children[c].node, key_length_in_bytes - 1)) {
                        return false;
                    }
                    // Child node was not emptied.
                    if (children[c].node) {
                        return true;
                    }
                } else {
                    delete children[c].value;
                }
                if (children_count == 5) {
                    auto new_node = create_node4();
                    new_node->children_count = node_16->children_count - 1;
                    detail::copy_all_but_one(keys.begin(), keys.end(), c,
                                             new_node->keys.begin());
                    detail::copy_all_but_one(children.begin(), children.end(), c,
                                             new_node->children.begin());
                    delete node_16;
                    *node = nullptr;
                } else {
                    node_16->children_count--;
                    detail::shift_left(keys.begin() + c, keys.begin() + children_count);
                    detail::shift_left(children.begin() + c,
                                       children.begin() + children_count);
                }

                return true;
            }

        } else if (children_count <= 48) {
            //auto node_48 = static_cast<node48*>(*node);
        } else {
            //auto node_256 = static_cast<node256*>(*node);
        }
        return false;
    }

public:
    adaptive_radix_tree() : root_(nullptr) {}

    ~adaptive_radix_tree() noexcept { destroy(root_, 0); }

    bool empty() const noexcept { return !root_; }

    bool insert(Key key, const Value& value) {
        return insert(key, value, &root_, sizeof(key));
    }

    const Value* find(const Key& key) const noexcept {
        return root_ ? find(key, *root_, 0) : nullptr;
    }

    bool erase(const Key& key) { return erase(key, &root_, sizeof(key)); }

    static void dump(const node_header* node, unsigned depth) {
        if (node) {
            const std::string prefix(depth, '.');
            const int children_count = node->children_count + 1;
            std::cout << prefix << children_count << '/';
            if (children_count <= 4) {
                std::cout << 4 << '|';
                const auto n4 = static_cast<const node4*>(node);
                for (int c = 0; c < children_count; ++c) {
                    std::cout << fmt::format("{0:02x}|", n4->keys[c]);
                }
                std::cout << '\n';
                if (depth < sizeof(Key) - 1) {
                    for (int c = 0; c < children_count; ++c) {
                        dump(n4->children[c].node, depth + 1);
                    }
                } else {
                    for (int c = 0; c < children_count; ++c) {
                        std::cout
                            << fmt::format(".{}({})\n", prefix, *n4->children[c].value);
                    }
                }
            } else if (children_count <= 16) {
                std::cout << 16 << '|';
                const auto n16 = static_cast<const node16*>(node);
                for (int c = 0; c < children_count; ++c) {
                    std::cout << fmt::format("{0:02x}|", n16->keys[c]);
                }
                std::cout << '\n';
                if (depth < sizeof(Key) - 1) {
                    for (int c = 0; c < children_count; ++c) {
                        dump(n16->children[c].node, depth + 1);
                    }
                } else {
                    for (int c = 0; c < children_count; ++c) {
                        std::cout
                            << fmt::format(".{}({})\n", prefix, *n16->children[c].value);
                    }
                }
            } else if (children_count <= 48) {
                std::cout << 48 << '|';
                const auto n48 = static_cast<const node48*>(node);
                for (int c = 0; c < 256; ++c) {
                    if (n48->child_index[c] != node48::none_index) {
                        std::cout << fmt::format("{0:02x}|", c);
                    }
                }
                std::cout << '\n';
                for (int c = 0; c < 256; ++c) {
                    if (n48->child_index[c] == node48::none_index) {
                        continue;
                    }
                    if (depth < sizeof(Key) - 1) {
                        dump(n48->children[n48->child_index[c]].node, depth + 1);
                    } else {
                        std::cout
                            << fmt::format(".{}({})\n", prefix,
                                           *n48->children[n48->child_index[c]].value);
                    }
                }
            } else {
                std::cout << 256 << '|';
                const auto n256 = static_cast<const node256*>(node);
                for (int c = 0; c < 256; ++c) {
                    if (n256->children[c].node) {
                        std::cout << fmt::format("{0:02x}|", c);
                    }
                }
                std::cout << '\n';
            }
        }
    }

    void dump() const { dump(root_, 0); }
};

}  // namespace stuff::container
