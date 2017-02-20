#ifndef NODEMANAGER_H_XH1HSC4Z
#define NODEMANAGER_H_XH1HSC4Z

#include <cstdint>

namespace traci
{

class LiteAPI;

class NodeManager
{
public:
    /**
     * Number of nodes currently managed by this manager
     * \return number of nodes
     */
    virtual std::size_t getNumberOfNodes() const = 0;

    /**
     * Access to lite API interface
     * \return API object
     */
    virtual LiteAPI* getLiteAPI() = 0;

    virtual ~NodeManager() = default;

private:
    //LiteAPI* m_api = nullptr;
};

} // namespace traci

#endif /* NODEMANAGER_H_XH1HSC4Z */

