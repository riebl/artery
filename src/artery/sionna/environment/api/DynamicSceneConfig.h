#pragma once

#include <artery/sionna/environment/api/SionnaAPI.h>

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace artery::sionna
{

// Queued dynamic scene edits for Sionna scene objects.
class DynamicSceneConfigProxy : public IDynamicSceneConfigProxy
{
public:
    explicit DynamicSceneConfigProxy(ISionnaAPI* api);

    // IDynamicSceneConfigProxy implementation.
    void edit() override;
    bool addReceiver(py::Receiver receiver) override;
    bool addTransmitter(py::Transmitter transmitter) override;
    void removeSceneItem(const std::string& id) override;
    std::unique_ptr<ITransformProxy> addObject(py::SceneObject object) override;
    std::unique_ptr<ITransformProxy> updateObject(const std::string& id) override;

private:
    class TransformProxy : public ITransformProxy
    {
    public:
        TransformProxy(DynamicSceneConfigProxy* owner, std::string id);

        // ITransformProxy implementation.
        void transform(std::function<void(py::SceneObject&)> function) override;

    private:
        DynamicSceneConfigProxy* owner_ = nullptr;
        std::string id_;
    };

    void addTransform(const std::string& id, std::function<void(py::SceneObject&)> function);
    bool containsSceneItem(const std::string& id) const;

private:
    ISionnaAPI* api_ = nullptr;
    std::vector<std::string> toRemove_;
    std::unordered_map<std::string, std::shared_ptr<py::SceneObject>> pendingObjects_;
    std::unordered_map<std::string, std::shared_ptr<py::SceneObject>> cachedObjects_;
    std::unordered_map<std::string, std::vector<std::function<void(py::SceneObject&)>>> transforms_;
};

}  // namespace artery::sionna
