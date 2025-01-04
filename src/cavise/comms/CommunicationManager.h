#pragma once

// STD
#include <mutex>
#include <queue>
#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <condition_variable>

// plog
#include <plog/Log.h>
#include <plog/Severity.h>
#include <plog/Initializers/RollingFileInitializer.h>

// zmq
#include <zmq.hpp>

// protos
#include <google/protobuf/message.h>


namespace cavise {

    /**
     * @brief Adapter for value type _ResultType_, holding either an error or a value.
     * @tparam ResultType value to hold
     */
    template<typename ResultType>
    class Result {
    public:

        /**
         * @brief Wraps error result, providing reason why.
         * Static constructor for _Result_ class, makes error-result
         * with error string.
         * @param message error string
         * @return Result constructed _Result_ class, holding a error
         */
        static Result wrapError(const std::string& message);

        /**
         * @brief Wraps correct result, passing arguments for constructing one.
         * Static constructor for _Result_ class, makes correct result with
         * arguments passed to constructor.
         * @tparam Args template arguments
         * @param args arguments passed to constructor
         * @return Result constructed _Result_ class, holding a correct result
         */
        template<typename... Args>
        static Result wrapResult(Args&&... args);

        /**
         * @brief Returns true if result holds error.
         * @return true if result is error
         */
        bool isError() const noexcept;

        /**
         * @brief Returns result, throws _std::bad_optional_access_ if state does not hold a value.
         * @return ResultType& reference to a value
         */
        ResultType& result();

        /**
         * @brief Returns error string, throws _std::bad_optional_access_ if state does not hold an error.
         * @return std::string error string
         */
        std::string error();

        /**
         * @brief Construct a new _Result_ object, holding a value.
         * @tparam Args template arguments
         * @param args arguments passed to constructor
         */
        template<typename... Args>
        Result(Args&&... args);

        /**
         * @brief Construct a new _Result_ object, holding an error.
         * @param message error string
         */
        Result(const std::string& message);

    private:
        std::optional<ResultType> result_;
        std::string error_;

    };

    struct Dummy {};

    /**
     * @brief Alias for _Result_, which does not attend to underlying type.
     * Use this when function/method needs to return void/error.
     */
    using BinaryResult = Result<Dummy>;

    /**
     * @brief Class to manage connection with OpenCDA.
     * @tparam ArteryMessageType Message type for Artery
     * @tparam OpencdaMessageType Message type for OpenCDA
     */
    template<typename ArteryMessageType, typename OpencdaMessageType>
    class CommunicationManager
        : public std::enable_shared_from_this<CommunicationManager<ArteryMessageType, OpencdaMessageType>> {
    private: struct Private { };
    public:

        static_assert(std::is_base_of<google::protobuf::Message, ArteryMessageType>::value, "Artery message should be derived from google::protobuf::Message");
        static_assert(std::is_base_of<google::protobuf::Message, OpencdaMessageType>::value, "OpenCDA message should be derived from google::protobuf::Message");

        using artery_message_t = std::unique_ptr<ArteryMessageType>;
        using opencda_message_t = std::unique_ptr<OpencdaMessageType>;

        /**
         * @brief Create shared pointer to _CommunicationManager_
         * @param address address of OpenCDA to connect this instance to
         * @param queueMaxSize max queue size for artery messages
         * @return std::shared_ptr<CommunicationManager> 
         */
        static std::shared_ptr<CommunicationManager> create(
            const std::string& address,
            std::size_t queueMaxSize
        );

        /**
         * @brief This constructor is not part of public API.
         */
        CommunicationManager(
            const std::string& address,
            std::size_t queueMaxSize,
            Private access
        );

        /**
         * @brief Initialize instance, connect to peer etc.
         * @return BinaryResult error wrapper for this method
         */
        BinaryResult initialize();

        /**
         * @brief Collect current message from OpenCDA
         * @return Result<opencda_message_t> result, holding collected message
         */
        Result<opencda_message_t> collect();

        /**
         * @brief Push new message to the queue for _cav_.
         * @param cav cav that owns the message
         * @param message message to add to the queue
         * @return BinaryResult error wrapper for this method
         */
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
ResultType& cavise::Result<ResultType>::result() {
    if (result_.has_value()) {
        return result_.value();
    }
    throw std::bad_optional_access();
}

template<typename ResultType>
std::string cavise::Result<ResultType>::error() {
    if (!result_.has_value()) {
        return error_;
    }
    throw std::bad_optional_access();
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

template<typename ArteryMessageType, typename OpencdaMessageType>
std::shared_ptr<cavise::CommunicationManager<ArteryMessageType, OpencdaMessageType>> cavise::CommunicationManager<ArteryMessageType, OpencdaMessageType>::create(
    const std::string& address,
    std::size_t queueMaxSize
) {
    return std::make_shared<CommunicationManager>(address, queueMaxSize, Private());
}

template<typename ArteryMessageType, typename OpencdaMessageType>
cavise::CommunicationManager<ArteryMessageType, OpencdaMessageType>::CommunicationManager(
    const std::string& address,
    std::size_t queueMaxSize,
    Private /* access */
)
    : qMaxSize_(queueMaxSize)
    , address_(address)
    , socket_(context_, zmq::socket_type::pair)
{}

template<typename ArteryMessageType, typename OpencdaMessageType>
cavise::BinaryResult cavise::CommunicationManager<ArteryMessageType, OpencdaMessageType>::initialize() {
    socket_.bind(address_);
    PLOG(plog::debug) << "communication manager: listening on " << address_;

    worker_ = std::make_unique<Worker>(std::thread(&CommunicationManager::run, this));
    PLOG(plog::debug) << "communication manager: running on separate thread, returning to caller";

    return BinaryResult::wrapResult();
}

template<typename ArteryMessageType, typename OpencdaMessageType>
cavise::Result<typename cavise::CommunicationManager<ArteryMessageType, OpencdaMessageType>::opencda_message_t> cavise::CommunicationManager<ArteryMessageType, OpencdaMessageType>::collect() {
    // might be not synchronized here, but it could only increase
    // so it save to assume this condition works
    std::unique_lock<std::mutex> unique_lock (worker_->mutex);
    if (!opencdaMessage_.get()) {
        PLOG(plog::debug) << "communication manager: no message to collect from opencda just yet, locking caller";

        worker_->cv.wait(unique_lock, [this]() {
            return opencdaMessage_.get();
        });
    }

    PLOG(plog::debug) << "communication manager: message from opencda collected!";
    auto message = std::make_unique<OpencdaMessageType>();
    message->CopyFrom(*opencdaMessage_);
    return Result<opencda_message_t>::wrapResult(std::move(message));
}

template<typename ArteryMessageType, typename OpencdaMessageType>
cavise::BinaryResult cavise::CommunicationManager<ArteryMessageType, OpencdaMessageType>::push(int cav, artery_message_t message) {
    {
        std::unique_lock<std::mutex> unique_lock (worker_->mutex);
        PLOG(plog::debug) << "communication manager: trying to push message onto artery queue...";

        std::queue<artery_message_t>& queue = arteryCavMapper_[cav];
        if (queue.size() >= qMaxSize_) {
            return BinaryResult::wrapError("queue is full, dropping message");
            PLOG(plog::debug) << "communication manager: queue is full for cav: " << cav;
        }

        queue.push(std::move(message));
        PLOG(plog::debug) << "communication manager: success! cav is " << cav;
    }

    worker_->cv.notify_all();
    return BinaryResult::wrapResult();
}

template<typename ArteryMessageType, typename OpencdaMessageType>
typename cavise::CommunicationManager<ArteryMessageType, OpencdaMessageType>::artery_message_t cavise::CommunicationManager<ArteryMessageType, OpencdaMessageType>::joinCav() {
    std::unique_lock<std::mutex> unique_lock (worker_->mutex);
    auto message = std::make_unique<ArteryMessageType>();

    // if (!arteryCavMapper_.size()) {
    //     worker_->cv.wait(unique_lock, [this]() {
    //         return arteryCavMapper_.size();
    //     });
    // }

    for (auto& pair : arteryCavMapper_) {
        if (!pair.second.size()) {
            worker_->cv.wait(unique_lock, [&pair]() {
                return pair.second.size();
            });
        }
        artery_message_t cav = std::move(pair.second.front());
        message->MergeFrom(*cav);
        pair.second.pop();
    }

    return message;
}

template<typename ArteryMessageType, typename OpencdaMessageType>
void cavise::CommunicationManager<ArteryMessageType, OpencdaMessageType>::run() {

    while (true) {
        if (worker_->abort) {
            PLOG(plog::debug) << "communication manager: aborting";
            return;
        }
        zmq::message_t incomingMsg;
        if (std::optional<std::size_t> bytes = socket_.recv(incomingMsg); !bytes.has_value()) {
            // EAGAIN
            PLOG(plog::debug) << "communication manager: returning from recv() with signal AGAIN. Is this a misconfiguration?";
            continue;
        } else {
            PLOG(plog::debug) << "communication manager: receiving " << bytes.value() << " bytes";
        }

        auto protoIncomingMessage = std::make_unique<OpencdaMessageType>();
        if (bool status = protoIncomingMessage->ParseFromArray(incomingMsg.data(), incomingMsg.size()); !status) {
            throw std::runtime_error("failed to parse OpenCDA message");
        }
        PLOG(plog::debug) << "communication manager: parsed OpenCDA message, pushing it to queue...";

        {
            std::unique_lock<std::mutex> locked (worker_->mutex);
            opencdaMessage_ = std::move(protoIncomingMessage);
            PLOG(plog::debug) << "communication manager: successfully pushed message onto a queue";
        }
        worker_->cv.notify_one();

        auto protoOutMessage = joinCav();
        PLOG(plog::debug) << "communication manager: joined cavs, artery message ready to be sent";

        zmq::message_t outMsg(protoOutMessage->ByteSizeLong());
        protoOutMessage->SerializeToArray(outMsg.data(), protoOutMessage->ByteSizeLong());
        if (std::optional<std::size_t> bytes = socket_.send(outMsg, zmq::send_flags::none); !bytes.has_value()) {
            // EAGAIN
            PLOG(plog::debug) << "communication manager: returning from send() with signal AGAIN. Is this a misconfiguration?";
            continue;
        } else {
            PLOG(plog::debug) << "communication manager: sending " << bytes.value() << " bytes";
        }

    }

}

template<typename ArteryMessageType, typename OpencdaMessageType>
cavise::CommunicationManager<ArteryMessageType, OpencdaMessageType>::Worker::Worker(std::thread&& thread) 
    : thread(std::move(thread))
    , abort(false)
{}

template<typename ArteryMessageType, typename OpencdaMessageType>
cavise::CommunicationManager<ArteryMessageType, OpencdaMessageType>::Worker::~Worker() {
    abort = true;
    thread.join();
}