#pragma once

#include <iterator>
#include <string_view>

#include <boost/iterator/iterator_facade.hpp>

namespace stuff {

enum class merge_adjacent_separators { yes, no };

namespace detail {

struct splitting_view_sentinel {};

class splitting_view_iterator
    : public boost::iterator_facade<splitting_view_iterator, std::string_view,
                                    boost::forward_traversal_tag, std::string_view> {
    const char* begin_;
    const char* end_;
    const char* nearest_split_;
    const char separator_;
    merge_adjacent_separators mode_;

    friend class boost::iterator_core_access;

    void increment() noexcept {
        if (nearest_split_ != end_) {
            begin_ = std::next(nearest_split_);
            if (mode_ == merge_adjacent_separators::yes) {
                while (begin_ != end_ && *begin_ == separator_) {
                    ++begin_;
                }
            }
            nearest_split_ = std::find(begin_, end_, separator_);
        } else {
            begin_ = nullptr;
        }
    }

    bool equal(const splitting_view_iterator& other) const noexcept {
        return begin_ == other.begin_ && end_ == other.end_ &&
               nearest_split_ == other.nearest_split_ && separator_ == other.separator_;
    }

    std::string_view dereference() const noexcept {
        return std::string_view(begin_, std::distance(begin_, nearest_split_));
    }

public:
    explicit splitting_view_iterator(const std::string_view& data, char separator,
                                     merge_adjacent_separators mode)
        : begin_(std::begin(data)),
          end_(std::end(data)),
          separator_(separator),
          mode_(mode) {
        nearest_split_ = std::find(begin_, end_, separator_);
    }

    bool operator==(const splitting_view_sentinel&) const noexcept {
        return begin_ == nullptr;
    }

    bool operator!=(const splitting_view_sentinel&) const noexcept {
        return begin_ != nullptr;
    }
};

}  // namespace detail

struct splitting_view {
    const std::string_view data_;
    const char separator_;
    const merge_adjacent_separators mode_;

public:
    splitting_view(std::string_view data, char separator, merge_adjacent_separators mode)
        : data_(data), separator_(separator), mode_(mode) {}

    detail::splitting_view_iterator begin() const noexcept {
        return detail::splitting_view_iterator{data_, separator_, mode_};
    }

    detail::splitting_view_sentinel end() const noexcept {
        return detail::splitting_view_sentinel{};
    }
};

splitting_view split(std::string_view data, char c,
                     merge_adjacent_separators mode = merge_adjacent_separators::no) {
    return splitting_view{data, c, mode};
}

}  // namespace stuff
