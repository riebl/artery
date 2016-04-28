#ifndef PORT_DISPATCHER_HPP_YZ0UTAUF
#define PORT_DISPATCHER_HPP_YZ0UTAUF

#include <vanetza/btp/data_interface.hpp>
#include <vanetza/btp/header.hpp>
#include <vanetza/common/hook.hpp>
#include <vanetza/geonet/transport_interface.hpp>
#include <vanetza/net/packet.hpp>
#include <list>
#include <unordered_map>

namespace vanetza
{

namespace btp
{

class PortDispatcher : public geonet::TransportInterface
{
public:
    class PromiscuousHook
    {
    public:
        /**
         * Called when a BTP packet is received, regardless of its destination port
         * \param indication BTP data indication
         * \param packet Read-only view of packet
         */
        virtual void tap_packet(const DataIndication&, const UpPacket&) = 0;
        virtual ~PromiscuousHook() {}
    };

    /**
     * Register a handler for incoming BTP-B (non-interactive) packets
     * \param port BTP-B destination port
     * \param interface Handler for given destination port
     * \note Use a nullptr as handler to disable dispatching of destination port
     * \note Only one handler can be registered per port
     */
    void set_non_interactive_handler(port_type, IndicationInterface*);

    /**
     * Add a hook listening for all incoming BTP packets
     * \param hook A hook implementing the promiscuous hook interface
     * \note Multiple hooks can be registered at a dispatcher at the same time
     */
    void add_promiscuous_hook(PromiscuousHook*);

    // Implementation of geonet::TransportInterface
    void indicate(const geonet::DataIndication&, std::unique_ptr<UpPacket>) override;

    /**
     * Hook invoked for all indicated but undispatched packets
     * \note BTP data indication can be nullptr
     */
    Hook<const geonet::DataIndication&, const btp::DataIndication*> hook_undispatched;

private:
    typedef std::unordered_map<port_type, IndicationInterface*> port_map;
    typedef std::list<PromiscuousHook*> hook_list;

    port_map m_non_interactive_handlers;
    hook_list m_promiscuous_hooks;
};

} // namespace btp
} // namespace vanetza

#endif /* PORT_DISPATCHER_HPP_YZ0UTAUF */

