// STD
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <stdexcept>
#include <thread>

// zmq
#include <zmq.hpp>

// plog
#include <plog/Log.h>
#include <plog/Severity.h>
#include <plog/Initializers/RollingFileInitializer.h>

// local
#include "CommunicationManager.h"

using namespace cavise;


std::shared_ptr<CommunicationManager> CommunicationManager::create(
    const std::string& address,
    std::size_t queueMaxSize
) {
    return std::make_shared<CommunicationManager>(address, queueMaxSize, Private());
}

CommunicationManager::CommunicationManager(
    const std::string& address,
    std::size_t queueMaxSize,
    Private access
)
    : qMaxSize_(queueMaxSize)
    , address_(address)
    , socket_(context_, zmq::socket_type::pair)
{}

BinaryResult CommunicationManager::initialize() {
    socket_.bind(address_);
    PLOG(plog::debug) << "communication manager: listening on " << address_;

    worker_ = std::make_unique<Worker>(std::thread(&CommunicationManager::run, this));
    PLOG(plog::debug) << "communication manager: running on separate thread, returning to caller";

    return BinaryResult::wrapResult();
}

Result<CommunicationManager::opencda_message_t> CommunicationManager::collect() {
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
    auto message = std::make_unique<structure_opencda::OpenCDA_message>();
    message->CopyFrom(*opencdaMessage_);
    return Result<opencda_message_t>::wrapResult(std::move(message));
}

BinaryResult CommunicationManager::push(int cav, artery_message_t message) {
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

CommunicationManager::artery_message_t CommunicationManager::joinCav() {
    std::unique_lock<std::mutex> unique_lock (worker_->mutex);
    auto message = std::make_unique<structure_artery::Artery_message>();

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

void CommunicationManager::run() {

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

        auto protoIncomingMessage = std::make_unique<structure_opencda::OpenCDA_message>();
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

CommunicationManager::Worker::Worker(std::thread&& thread) 
    : thread(std::move(thread))
    , abort(false)
{}

CommunicationManager::Worker::~Worker() {
    abort = true;
    thread.join();
}