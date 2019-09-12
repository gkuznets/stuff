#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

#include <stuff/meta/named_template_params.hpp>

#ifdef STUFF_DEBUG
#include <stuff/assert.hpp>
#include <thread>
#endif  // STUFF_DEBUG

namespace stuff {

namespace detail {

struct single_chunk_tag {};

template <typename T>
class basic_storage {
    T* ptr_ = nullptr;
    std::atomic<std::uint64_t> counter_;

public:
    constexpr basic_storage() : counter_(1) {}

    uint64_t release_one() noexcept { return --counter_; }
};

template <typename Counter>
struct control_block {
    void* ptr;
    Counter counter;
    std::function<void(control_block*)> deleter;

    control_block(void* ptr, Counter counter, std::function<void(control_block*)> deleter) noexcept :
        ptr(ptr), counter(counter), deleter(std::move(deleter)) {}
};

template <typename T, typename Counter = std::uint64_t>
class single_threaded_storage {
    using cb = control_block<Counter>;

    T* ptr_ = nullptr;
    cb* cb_ = nullptr;

#ifdef STUFF_DEBUG
    std::thread::id owner_thread_;
#endif  // STUFF_DEBUG

    void check_owner_thread() const {
#ifdef STUFF_DEBUG
        STUFF_ASSERT(owner_thread_ == std::this_thread::get_id());
#endif
    }

    void retain() noexcept {
        if (cb_) {
            ++(cb_->counter);
        }
    }

    void release() noexcept {
        if (cb_ && !--(cb_->counter)) {
            auto deleter = std::move(cb_->deleter);
            deleter(cb_);
        }
    }

public:
#ifdef STUFF_DEBUG
    single_threaded_storage() noexcept : owner_thread_(std::this_thread::get_id()) {}
#else
    single_threaded_storage() noexcept = default;
#endif  // STUFF_DEBUG

    explicit single_threaded_storage(T* ptr)
        : ptr_(ptr),
          cb_(ptr ? new cb{ptr, 1,
                           [](cb* pcb) {
                               delete static_cast<T*>(pcb->ptr);
                               delete pcb;
                           }}
                  : nullptr)
#ifdef STUFF_DEBUG
          ,
          owner_thread_(std::this_thread::get_id())
#endif  // STUFF_DEBUG
    {
    }

    template <typename Y>
    single_threaded_storage(const single_threaded_storage<Y, Counter>& other,
                            T* ptr) noexcept
        : ptr_(ptr),
          cb_(other.cb_)
#ifdef STUFF_DEBUG
          ,
          owner_thread_(std::this_thread::get_id())
#endif  // STUFF_DEBUG
    {
        retain();
    }

    template <typename Y, std::enable_if_t<std::is_convertible_v<Y*, T*>, int> _>
    explicit single_threaded_storage(
        const single_threaded_storage<Y, Counter>& other) noexcept
        : single_threaded_storage(other, other.ptr_) {}

    template <typename... Args>
    explicit single_threaded_storage(single_chunk_tag, Args&&... args)
#ifdef STUFF_DEBUG
        : owner_thread_(std::this_thread::get_id())
#endif  // STUFF_DEBUG
    {
        using some = std::pair<cb, T>;
        auto psome = std::make_unique<some>(
            std::piecewise_construct_t{},
            std::make_tuple<void*, Counter, std::function<void(cb*)>>(nullptr, 1, [](cb* pcb) {
                                                        delete reinterpret_cast<some*>(
                                                            reinterpret_cast<char*>(pcb) -
                                                            offsetof(some, first));
                                                    }),
            std::make_tuple<Args...>(std::forward<Args>(args)...));
        ptr_ = &(psome->second);
        cb_ = &(psome->first);
        psome.release();
    }

    ~single_threaded_storage() {
        release();
        check_owner_thread();
    }

    single_threaded_storage& operator=(const single_threaded_storage& other) noexcept {
        // no need to check if assigning from self
        if (cb_ != other.cb_) {
            release();
            cb_ = other.cb_;
            ptr_ = other.ptr_;
            retain();
        }
        check_owner_thread();
        return *this;
    }

    T* get() const noexcept { return ptr_; }

    long use_count() const noexcept { return cb_ ? cb_->counter : 0; }

    void reset() noexcept {
        if (cb_) {
            release();
            cb_ = nullptr;
            ptr_ = nullptr;
        }
    }

    void swap(single_threaded_storage& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(cb_, other.cb_);
    }
};

};  // namespace detail

DECLARE_VALUE_SELECTOR(single_threaded, bool);
DECLARE_VALUE_SELECTOR(with_weak_pointers, bool);

template <typename T, typename... Params>
class shared_ptr {
    static constexpr auto single_threaded_ =
        stuff::meta::get_v<single_threaded<false>, Params...>;
    using storage =
        std::conditional_t<single_threaded_, detail::single_threaded_storage<T>,
                           detail::basic_storage<T>>;

    storage storage_;

    template <typename... Args>
    shared_ptr(detail::single_chunk_tag, Args... args)
        : storage_(detail::single_chunk_tag{}, std::forward<Args>(args)...) {}

public:
    using element_type = T;

    constexpr shared_ptr() noexcept = default;
    constexpr shared_ptr(std::nullptr_t) noexcept {}

    template <typename Y>
    explicit shared_ptr(Y* ptr) : storage_(ptr) {}

    template <typename Y>
    shared_ptr(const shared_ptr<Y, Params...>& other, element_type* ptr) noexcept
        : storage_(other.storage_, ptr) {}

    template <typename U>
    shared_ptr(const shared_ptr<U, Params...>& other) noexcept
        : storage_(other.storage_) {}

    shared_ptr& operator=(const shared_ptr& other) noexcept {
        if (this != &other) {
            storage_ = other.storage_;
        }
        return *this;
    }

    template <typename Y>
    shared_ptr& operator=(const shared_ptr<Y, Params...>& other) noexcept {
        if (this != &other) {
            storage_ = other.storage_;
        }
        return *this;
    }

    element_type* get() const noexcept { return storage_.get(); }

    element_type& operator*() const noexcept { return *get(); }

    element_type* operator->() const noexcept { return get(); }

    long use_count() const noexcept { return storage_.use_count(); }

    bool unique() const noexcept { return storage_.use_count() == 1; }

    explicit operator bool() const noexcept { return get() != nullptr; }

    void reset() noexcept { storage_.reset(); }

    void swap(shared_ptr& other) noexcept {
        storage_.swap(other.storage_);
    }

    // Friends

    template <typename Y, typename... Paramz, typename... Args>
    friend shared_ptr<Y, Paramz...> make_shared(Args&&... args);

};  // namespace stuff

template <typename Y, typename... Params, typename... Args>
shared_ptr<Y, Params...> make_shared(Args&&... args) {
    return shared_ptr<Y, Params...>(detail::single_chunk_tag{},
                                    std::forward<Args>(args)...);
}

}  // namespace stuff
