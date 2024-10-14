#pragma once

#include <mutex>
#include <memory>
#include <utility>
#include <stdexcept>

namespace cavise {

    template<typename Class>
    class SingletonHolder {
    public:

        template<typename... Args>
        void initialize(Args&&... args);

        bool initalized() const noexcept;
        Class& getInstance();

    private:
        static std::unique_ptr<Class> instance_;
        static std::mutex lock_;

    };

}

template<typename Class>
std::unique_ptr<Class> cavise::SingletonHolder<Class>::instance_ = nullptr;

template<typename Class>
std::mutex cavise::SingletonHolder<Class>::lock_ {};

template<typename Class>
template<typename... Args>
void cavise::SingletonHolder<Class>::initialize(Args&&... args) {
    std::unique_lock<std::mutex> locked_ (lock_);

    if (instance_) {
        throw std::runtime_error("singleton was initalized more than once!");
    }
    instance_ = std::make_unique<Class>(std::forward<Args...>(args...));
}

template<typename Class>
Class& cavise::SingletonHolder<Class>::getInstance() {
    std::unique_lock<std::mutex> locked_ (lock_);

    if (!instance_) {
        throw std::runtime_error("singleton was not initalized!");
    }
    return *instance_;
}

template<typename Class>
bool cavise::SingletonHolder<Class>::initalized() const noexcept {
    std::unique_lock<std::mutex> locked_ (lock_);

    return instance_.get();
}