#include "DynamicSceneConfig.h"

#include <omnetpp/cexception.h>

#include <algorithm>
#include <utility>

using namespace artery::sionna;

DynamicSceneConfigProxy::DynamicSceneConfigProxy(ISionnaAPI* api) : api_(api)
{
}

std::unique_ptr<IDynamicSceneConfigProxy::ITransformProxy> DynamicSceneConfigProxy::addObject(py::SceneObject object)
{
    auto id = object.name();

    pendingObjects_.insert_or_assign(id, std::make_shared<py::SceneObject>(std::move(object)));
    toRemove_.erase(std::remove(toRemove_.begin(), toRemove_.end(), id), toRemove_.end());
    return std::make_unique<TransformProxy>(this, std::move(id));
}

bool DynamicSceneConfigProxy::addTransmitter(py::Transmitter transmitter)
{
    if (api_ == nullptr) {
        throw omnetpp::cRuntimeError("cannot add Sionna transmitter: Sionna API is not configured");
    }

    auto id = transmitter.name();
    if (containsSceneItem(id)) {
        return false;
    }

    api_->scene().add(transmitter);
    return true;
}

bool DynamicSceneConfigProxy::addReceiver(py::Receiver receiver)
{
    if (api_ == nullptr) {
        throw omnetpp::cRuntimeError("cannot add Sionna receiver: Sionna API is not configured");
    }

    auto id = receiver.name();
    if (containsSceneItem(id)) {
        return false;
    }

    api_->scene().add(receiver);
    return true;
}

std::unique_ptr<IDynamicSceneConfigProxy::ITransformProxy> DynamicSceneConfigProxy::updateObject(const std::string& id)
{
    return std::make_unique<TransformProxy>(this, id);
}

void DynamicSceneConfigProxy::edit()
{
    if (api_ == nullptr) {
        throw omnetpp::cRuntimeError("cannot edit Sionna scene: Sionna API is not configured");
    }

    std::vector<py::SceneObject> toAdd;
    for (const auto& [_, object] : pendingObjects_) {
        toAdd.push_back(*object);
    }

    if (!toAdd.empty() || !toRemove_.empty()) {
        api_->scene().edit(toAdd, toRemove_);
    }
    for (auto& [id, object] : pendingObjects_) {
        cachedObjects_.insert_or_assign(id, object);
    }

    for (const auto& id : toRemove_) {
        cachedObjects_.erase(id);
    }

    for (auto& [id, callbacks] : transforms_) {
        auto found = cachedObjects_.find(id);
        if (found == cachedObjects_.end() || !found->second) {
            continue;
        }

        for (auto& callback : callbacks) {
            callback(*found->second);
        }
    }

    toRemove_.clear();
    pendingObjects_.clear();
    transforms_.clear();
}

DynamicSceneConfigProxy::TransformProxy::TransformProxy(DynamicSceneConfigProxy* owner, std::string id) : owner_(owner), id_(std::move(id))
{
}

void DynamicSceneConfigProxy::TransformProxy::transform(std::function<void(py::SceneObject&)> function)
{
    if (owner_ != nullptr) {
        owner_->addTransform(id_, std::move(function));
    }
}

void DynamicSceneConfigProxy::addTransform(const std::string& id, std::function<void(py::SceneObject&)> function)
{
    transforms_[id].push_back(std::move(function));
}

bool DynamicSceneConfigProxy::containsSceneItem(const std::string& id) const
{
    if (pendingObjects_.contains(id) || cachedObjects_.contains(id)) {
        return true;
    }

    return api_ != nullptr && !std::holds_alternative<std::monostate>(api_->scene().get(id));
}

void DynamicSceneConfigProxy::removeSceneItem(const std::string& id)
{
    if (api_ == nullptr) {
        throw omnetpp::cRuntimeError("cannot remove Sionna scene item: Sionna API is not configured");
    }

    const auto item = api_->scene().get(id);
    std::visit(
        [this, &id](const auto& value) {
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, std::monostate>) {
                if (pendingObjects_.erase(id) > 0) {
                    transforms_.erase(id);
                }
            } else if constexpr (std::is_same_v<T, py::SceneObject>) {
                if (pendingObjects_.erase(id) == 0 && cachedObjects_.contains(id)) {
                    toRemove_.push_back(id);
                }
                transforms_.erase(id);
            } else {
                api_->scene().remove(id);
            }
        },
        item);
}
