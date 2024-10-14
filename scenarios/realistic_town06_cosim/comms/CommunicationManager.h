#pragma once

// STD
#include <mutex>
#include <queue>
#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <optional>
#include <condition_variable>

// zmq
#include <zmq.hpp>

// protos
#include <cavise/artery.pb.h>
#include <cavise/opencda.pb.h>


namespace cavise {

    template<typename ResultType>
    class Result {
    public:
        static Result wrapError(const std::string& message);

        template<typename... Args>
        static Result wrapResult(Args&&... args);

        bool isError() const noexcept;
        ResultType& result() noexcept;
        std::string error() noexcept;

        template<typename... Args>
        Result(Args&&... args);

        Result(const std::string& message);

    private:
        std::optional<ResultType> result_;
        std::string error_;

    };

    struct Dummy { };
    using BinaryResult = Result<Dummy>;

    class CommunicationManager
        : public std::enable_shared_from_this<CommunicationManager> {
    private: struct Private { };
    public:

        using artery_message_t = std::unique_ptr<structure_artery::Artery_message>;
        using opencda_message_t = std::unique_ptr<structure_opencda::OpenCDA_message>;

        static std::shared_ptr<CommunicationManager> create(
            const std::string& address,
            std::size_t queueMaxSize
        );

        CommunicationManager(
            const std::string& address,
            std::size_t queueMaxSize,
            Private access
        );

        BinaryResult initialize();

        // sync calls
        Result<opencda_message_t> collect();
        BinaryResult push(int cav, artery_message_t message);

    private:
        artery_message_t joinCav();
        void run();

    private:

        struct Worker {

            Worker(std::thread&& thread);
            ~Worker();

            std::mutex mutex;
            std::thread thread;
            std::condition_variable cv;
            std::atomic<bool> abort;
        };

        std::unique_ptr<Worker> worker_;

        std::size_t qMaxSize_;
        std::unordered_map<int, std::queue<artery_message_t>> arteryCavMapper_;

        // maybe make it a bit different?
        opencda_message_t opencdaMessage_;

        std::string address_;

        zmq::context_t context_;
        zmq::socket_t socket_;

    };

} // namespace cavise


template<typename ResultType>
cavise::Result<ResultType> cavise::Result<ResultType>::wrapError(const std::string& message) {
    return Result<ResultType>(message);
}

template<typename ResultType>
template<typename... Args>
cavise::Result<ResultType> cavise::Result<ResultType>::wrapResult(Args&&... args) {
    return Result<ResultType>(std::forward<Args>(args)...);
}

template<typename ResultType>
bool cavise::Result<ResultType>::isError() const noexcept {
    return !result_.has_value() && error_.size() > 0;
}

template<typename ResultType>
ResultType& cavise::Result<ResultType>::result() noexcept {
    return result_.value();
}

template<typename ResultType>
std::string cavise::Result<ResultType>::error() noexcept {
    return error_;
}

template<typename ResultType>
template<typename... Args>
cavise::Result<ResultType>::Result(Args&&... args) 
    : result_(std::forward<Args>(args)...)
    , error_("")
{}

template<typename ResultType>
cavise::Result<ResultType>::Result(const std::string& message) 
    : error_(message)
    , result_(std::nullopt)
{}
