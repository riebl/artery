#ifndef ACCESS_CONTROL_HPP_OXJ73CEM
#define ACCESS_CONTROL_HPP_OXJ73CEM

#include <vanetza/common/clock.hpp>
#include <vanetza/common/hook.hpp>
#include <vanetza/dcc/data_request.hpp>
#include <vanetza/dcc/interface.hpp>
#include <vanetza/net/chunk_packet.hpp>
#include <memory>

namespace vanetza
{

// forward declarations
class ChunkPacket;
namespace access { class Interface; }

namespace dcc
{

// forward declarations
struct DataRequest;
class Scheduler;

class AccessControl : public RequestInterface
{
public:
    AccessControl(Scheduler&, access::Interface&);
    void request(const DataRequest&, std::unique_ptr<ChunkPacket>) override;

    /**
     * \brief Drop packets arriving too frequent (Tx delay enforcement)
     *
     * Packets arriving before termination of scheduler's transmission delay
     * are dropped when enforcement is active. Otherwise all packets pass.
     * Packet drop is notified via hook_dropped.
     * \param bool enforce Enforce transmission delays if true
     */
    void drop_excess(bool enforce) { m_drop_excess = enforce; }

    Hook<const DataRequest&, std::unique_ptr<ChunkPacket>> hook_dropped;

private:
    Scheduler& m_scheduler;
    access::Interface& m_access;
    bool m_drop_excess;
};

} // namespace dcc
} // namespace vanetza

#endif /* ACCESS_CONTROL_HPP_OXJ73CEM */

