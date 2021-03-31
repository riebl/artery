#ifndef NODEMANAGER_H_XH1HSC4Z
#define NODEMANAGER_H_XH1HSC4Z

#include <cstdint>
#include <memory>

namespace traci
{

class API;

class NodeManager
{
public:
    /**
     * Number of nodes currently managed by this manager
     * \return number of nodes
     */
    virtual std::size_t getNumberOfNodes() const = 0;

    /**
     * Access to TraCI API interface
     * \return API object
     */
    virtual std::shared_ptr<API> getAPI() = 0;

    virtual ~NodeManager() = default;
};

} // namespace traci

#endif /* NODEMANAGER_H_XH1HSC4Z */

