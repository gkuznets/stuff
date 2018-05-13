#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <type_traits>

#include <stuff/assert.hpp>
#include <stuff/exception.hpp>

namespace stuff::container {

template <typename T>
class any_small_vector {
    T* begin_;
    T* end_;
    std::int64_t capacity_;

    void destruct() noexcept {
        destruct(begin_, end_);

        if (!inplace()) {
            delete[] reinterpret_cast<storage_t*>(begin_);
        }
    }

    bool inplace() const noexcept { return capacity_ <= 0; }

    template <typename RandomIter>
    void init(RandomIter first, RandomIter last, std::random_access_iterator_tag) {
        STUFF_ASSERT(begin_ == end_);

        const size_type range_size = std::distance(first, last);
        std::unique_ptr<storage_t[]> buffer;
        if (range_size > capacity()) {
            buffer.reset(new storage_t[range_size]);
            capacity_ = range_size;
            begin_ = end_ = reinterpret_cast<T*>(buffer.get());
        }
        try {
            while (first != last) {
                new (end_) T(*first);
                ++end_;
                ++first;  // this might throw
            }
        } catch (...) {
            destruct(begin_, end_);
            throw;
        }
        buffer.release();
    }

    template <typename InputIter, typename NonRandomAccessIteratorTag>
    void init(InputIter first, InputIter last, NonRandomAccessIteratorTag) {
        STUFF_ASSERT(begin_ == end_);

        try {
            while (first != last) {
                push_back(*first);
                ++first;
            }
        } catch (...) {
            destruct(begin_, end_);
            throw;
        }
    }

    template <typename RandomIter>
    void assign(RandomIter first, RandomIter last, std::random_access_iterator_tag) {
        const auto range_size = std::distance(first, last);
        if (range_size <= capacity()) {
            auto it = begin_;
            while (it != end_ && first != last) {
                *it = *first;
                ++it, ++first;
            }
            if (it != end_) {
                destruct(it, end_);
                end_ = it;
                return;
            }
            while (first != last) {
                new (end_) T(*first);
                ++end_, ++first;
            }
        } else {
            std::unique_ptr<storage_t[]> buffer{new storage_t[range_size]};
            destruct();
            begin_ = end_ = reinterpret_cast<T*>(buffer.get());
            capacity_ = range_size;
            buffer.release();
            while (first != last) {
                new (end_) T(*first);
                ++end_, ++first;  // this might throw
            }
        }
    }

    template <typename Value>
    void do_push_back(Value&& value) {
        if (size() < capacity()) {
            new (end_) T(std::forward<Value>(value));
            ++end_;
        } else {
            const auto new_capacity = std::max<size_type>(1, capacity() * 2);
            std::unique_ptr<storage_t[]> buffer{new storage_t[new_capacity]};
            T* new_begin{reinterpret_cast<T*>(buffer.get())};
            T* new_end = new_begin;
            try {
                for (auto& t : *this) {
                    new (new_end) T(std::move_if_noexcept(t));
                    ++new_end;
                }
                new (new_end) T(std::forward<Value>(value));
                ++new_end;
            } catch (...) {
                destruct(new_begin, new_end);
                throw;
            }

            destruct();
            begin_ = new_begin;
            end_ = new_end;
            capacity_ = new_capacity;
            buffer.release();
        }
    }

protected:
    using storage_t = std::aligned_storage_t<sizeof(T), alignof(T)>;

    any_small_vector(T* begin, std::int64_t capacity) noexcept
        : begin_(begin), end_(begin), capacity_(capacity) {}

    any_small_vector(T* begin, std::int64_t capacity, std::int64_t count) noexcept
        : begin_(begin), end_(begin), capacity_(capacity) {
        std::unique_ptr<storage_t[]> buffer;
        if (count > capacity_) {
            buffer.reset(new storage_t[count]);
            capacity_ = count;
            begin_ = end_ = reinterpret_cast<T*>(buffer.get());
        }
        try {
            while (count) {
                new (end_) T();
                ++end_;
                --count;
            }
        } catch (...) {
            destruct(begin_, end_);
            throw;
        }
        buffer.release();
    }

    template <typename InputIter>
    any_small_vector(T* begin, std::int64_t capacity, InputIter first, InputIter last)
        : begin_(begin), end_(begin), capacity_(capacity) {
        init(first, last, typename std::iterator_traits<InputIter>::iterator_category{});
    }

    static void destruct(T* begin, T* end) noexcept {
        while (begin != end) {
            begin->~T();
            ++begin;
        }
    }

public:
    using value_type = T;
    using size_type = std::int64_t;
    using reference = T&;
    using const_reference = const T&;
    using iterator = T*;
    using const_iterator = const T*;  // ?

    ~any_small_vector() { destruct(); }

    template <typename InputIter>
    void assign(InputIter first, InputIter last) {
        assign(first, last,
               typename std::iterator_traits<InputIter>::iterator_category{});
    }

    void assign(std::initializer_list<T> ilist) { assign(ilist.begin(), ilist.end()); }

    //@{
    //! \brief Returns a reference to the element at specified location pos, with
    //! bounds checking.
    //!
    //! If pos is not within the range of the vector, an exception of type
    //! std::out_of_range is thrown.
    //!
    //! \param pos — position of the element to return
    reference at(size_type pos) {
        if (pos >= size()) {
            STUFF_THROW(std::out_of_range, "...");
        }
        return *(begin_ + pos);
    }
    const_reference at(size_type pos) const {
        if (pos >= size()) {
            STUFF_THROW(std::out_of_range, "...");
        }
        return *(begin_ + pos);
    }
    //@}

    //@{
    //! \brief Returns a reference to the element at specified location pos. No bounds
    //! checking is performed.
    //!
    //! \param pos — position of the element to return
    reference operator[](size_type pos) {
        STUFF_CHECK_PRECONDITION(pos < size());

        return *(begin_ + pos);
    }
    const_reference operator[](size_type pos) const {
        STUFF_CHECK_PRECONDITION(pos < size());

        return *(begin_ + pos);
    }
    //@}

    //@{
    //! \brief Returns a reference to the first element in the vector.
    [[nodiscard]] reference front() {
        STUFF_CHECK_PRECONDITION(!empty());

        return *begin_;
    }

        [[nodiscard]] const_reference front() const {
        STUFF_CHECK_PRECONDITION(!empty());
        return *begin_;
    }
    //@}

    //@{
    //! \brief Returns a reference to the last element in the vector.
    //!
    //! Calling back on an empty vector is undefined.
    [[nodiscard]] reference back() {
        STUFF_CHECK_PRECONDITION(!empty());

        return *std::prev(end_);
    }

        [[nodiscard]] const_reference back() const {
        STUFF_CHECK_PRECONDITION(!empty());

        return *std::prev(end_);
    }
    //@}

    iterator begin() noexcept { return begin_; }
    iterator end() noexcept { return end_; }

    const_iterator begin() const noexcept { return begin_; }
    const_iterator end() const noexcept { return end_; }

    const_iterator cbegin() const noexcept { return begin_; }
    const_iterator cend() const noexcept { return end_; }

    //! \brief Returns the number of elements that the vector has currently allocated
    //! space for.
    size_type capacity() const noexcept { return capacity_ > 0 ? capacity_ : -capacity_; }

    void shrink_to_fit(){/* do nothing yet*/}

        //! \brief Checks if the vector has no elements.
        [[nodiscard]] bool empty() const noexcept {
        return begin_ == end_;
    }

    //! \brief Removes all elements from the vector.
    //!
    //! Leaves the capacity() unchanged.
    void clear() noexcept {
        destruct(begin_, end_);
        end_ = begin_;
    }

    //! \brief Returns the number of elements in the vector.
    [[nodiscard]] size_type size() const noexcept { return end_ - begin_; }

    //! \brief Resizes the vector to contain new_size elements.
    //!
    //! If the current size is greater than new_size, the vector is reduced to its
    //! first new_size elements. Resizing to smaller size does not invalidate
    //! iterators.
    void resize(std::int64_t new_size) {
        if (new_size > size()) {
            if (new_size > capacity()) {
                std::unique_ptr<storage_t[]> buffer{new storage_t[new_size]};
                T* new_begin{reinterpret_cast<T*>(buffer.get())};
                T* new_end = new_begin;
                try {
                    for (auto& t : *this) {
                        new (new_end) T(std::move_if_noexcept(t));
                        ++new_end;
                    }
                    while (new_end != new_begin + new_size) {
                        new (new_end) T();
                        ++new_end;
                    }
                } catch (...) {
                    destruct(new_begin, new_end);
                    throw;
                }
                destruct();
                begin_ = new_begin;
                end_ = new_end;
                capacity_ = new_size;
                buffer.release();
            } else {
                T* const new_end = end_ + (new_size - size());
                T* pos = end_;
                try {
                    while (pos != new_end) {
                        new (pos) T();
                        ++pos;
                    }
                } catch (...) {
                    destruct(end_, pos);
                    throw;
                }
                end_ = new_end;
            }
        } else {
            T* const new_end = end_ - (size() - new_size);
            destruct(new_end, end_);
            end_ = new_end;
        }
    }

    iterator erase(const_iterator pos) {
        STUFF_CHECK_PRECONDITION(pos >= begin_ && pos < end_);
        for (auto it = const_cast<iterator>(pos); it + 1 != end_; ++it) {
            *it = *(it + 1);
        }
        --end_;
        end_->~T();
        return const_cast<iterator>(pos);
    }

    iterator erase(const_iterator first, const_iterator last) {
        STUFF_CHECK_PRECONDITION(first >= begin_ && first <= last && last <= end_);
        const auto count = last - first;
        for (auto it = const_cast<iterator>(first); it + count != end_; ++it) {
            *it = *(it + count);
        }
        destruct(end_ - count, end_);
        end_ -= count;
        return const_cast<iterator>(first);
    }

    void push_back(const T& value) { do_push_back(value); }

    void push_back(T&& value) { do_push_back(std::move(value)); }

    //! \brief Removes the last element of the vector.
    void pop_back() {
        STUFF_CHECK_PRECONDITION(!empty());

        --end_;
        end_->~T();
    }
};

//! Small vector
template <typename T, std::int64_t n>
class small_vector : public any_small_vector<T> {
    using base = any_small_vector<T>;

    using typename base::storage_t;
    storage_t data_[n];

    T* data_begin() noexcept { return reinterpret_cast<T*>(data_); }

public:
    using typename base::const_iterator;
    using typename base::const_reference;
    using typename base::iterator;
    using typename base::reference;
    using typename base::size_type;
    using typename base::value_type;

    small_vector() noexcept : base(data_begin(), -n) {}

    explicit small_vector(size_type count) : base(data_begin(), -n, count) {}

    template <class InputIter>
    small_vector(InputIter first, InputIter last) : base(data_begin(), -n, first, last) {}

    //@{
    small_vector(const small_vector& other) : small_vector(other.begin(), other.end()) {}

    template <std::int64_t m>
    small_vector(const small_vector<T, m>& other)
        : small_vector(other.begin(), other.end()) {}
    //@}

    //! Constructs the vector with the contents of the initializer list init.
    small_vector(std::initializer_list<T> init)
        : small_vector(init.begin(), init.end()) {}

    small_vector& operator=(const small_vector& other) {
        assign(other.begin(), other.end());
        return *this;
    }

    template <std::int64_t m>
    small_vector& operator=(const small_vector<T, m>& other) {
        assign(other.begin(), other.end());
        return *this;
    }

    small_vector& operator=(std::initializer_list<T> ilist) {
        assign(ilist.begin(), ilist.end());
        return *this;
    }
};

template <typename T, std::int64_t n, std::int64_t m>
bool operator==(const small_vector<T, n>& lhs, const small_vector<T, m>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T, std::int64_t n, std::int64_t m>
bool operator!=(const small_vector<T, n>& lhs, const small_vector<T, m>& rhs) {
    return !(lhs == rhs);
}

}  // namespace stuff::container
