#ifndef ARTERY_SERVICETABLE_H_
#define ARTERY_SERVICETABLE_H_

#include "artery/application/NetworkInterface.h"

namespace artery
{
    class ItsG5BaseService;
    class IndicationInterfaceAdapter;
    class PromiscuousHookAdapter;

    class ServiceTable {
        public:
            using port_type = uint16_t;
            using PortInfoMap = std::map<NetworkInterface*, port_type>;

            class ServiceEntry {
                public:
                    const ItsG5BaseService* mService;
                    NetworkInterface& mInterface;

                    ServiceEntry(ItsG5BaseService* service, NetworkInterface& interface)
                        : mService(service),
                        mInterface(interface)
                    {}
            };

            class IndicationServiceEntry : public ServiceEntry {
                public:
                    std::unique_ptr<IndicationInterfaceAdapter> mIndicationAdapter;
                    port_type mPort;

                    IndicationServiceEntry(ItsG5BaseService* service, port_type port, NetworkInterface& interface, IndicationInterfaceAdapter* adapter)
                        : ServiceEntry(service, interface)
                          , mIndicationAdapter(adapter)
                          , mPort(port) {}
            };

            class PromiscuousServiceEntry : public ServiceEntry {
                public:
                    std::unique_ptr<PromiscuousHookAdapter> mPromiscuousAdapter;

                    PromiscuousServiceEntry(ItsG5BaseService* service, NetworkInterface& interface, PromiscuousHookAdapter* adapter)
                        : ServiceEntry(service, interface)
                          , mPromiscuousAdapter(adapter) {}
            };

            using IndicationServiceTableType = boost::multi_index_container<
                std::unique_ptr<IndicationServiceEntry>,
                boost::multi_index::indexed_by<
                    boost::multi_index::ordered_non_unique<
                        boost::multi_index::member<ServiceEntry, const ItsG5BaseService*, &ServiceEntry::mService>
                    >,
                    boost::multi_index::ordered_non_unique<
                        boost::multi_index::member<IndicationServiceEntry, port_type, &IndicationServiceEntry::mPort>
                    >
                >
            >;

            using PromiscuousServiceTableType = boost::multi_index_container<
                std::unique_ptr<PromiscuousServiceEntry>,
                boost::multi_index::indexed_by<
                    boost::multi_index::ordered_non_unique<
                        boost::multi_index::member<ServiceEntry, const ItsG5BaseService*, &ServiceEntry::mService>
                    >
                >
            >;

            void insert(ItsG5BaseService* service, NetworkInterface& interface, PromiscuousHookAdapter* adapter)
            {
                auto entry = new PromiscuousServiceEntry(service, interface, adapter);
                mPromiscuousTable.insert(std::unique_ptr<PromiscuousServiceEntry>(entry));
            }

            void insert(ItsG5BaseService* service, port_type port, NetworkInterface& interface, IndicationInterfaceAdapter* adapter)
            {
                auto entry = new IndicationServiceEntry(service, port, interface, adapter);
                mIndicationTable.insert(std::unique_ptr<IndicationServiceEntry>(entry));
            }

            PortInfoMap getPortsForService(const ItsG5BaseService* service) const {
                PortInfoMap ports;
                auto res = mIndicationTable.get<0>().equal_range(service);
                for (auto& entry : boost::make_iterator_range(res)) {
                    ports.emplace(&entry->mInterface, entry->mPort);
                }
                return ports;
            }

        private:
            IndicationServiceTableType mIndicationTable;
            PromiscuousServiceTableType mPromiscuousTable;
    };
} // namespace artery

#endif
