#ifndef ARTERY_NETWORKINTERFACETABLE_H_
#define ARTERY_NETWORKINTERFACETABLE_H_

#include "artery/application/NetworkInterface.h"

namespace artery
{
    class NetworkInterfaceTable {
        public:
            using InterfaceList = std::list<NetworkInterface*>;

            using NetworkInterfaceTableType = boost::multi_index_container<
                std::unique_ptr<NetworkInterface>,
                boost::multi_index::indexed_by<
                    boost::multi_index::ordered_unique<
                        boost::multi_index::identity<std::unique_ptr<NetworkInterface>>
                    >,
                    boost::multi_index::ordered_non_unique<
                        boost::multi_index::member<NetworkInterface, Channel, &NetworkInterface::channel>
                    >,
                    boost::multi_index::ordered_unique<
                        boost::multi_index::member<NetworkInterface, Router*, &NetworkInterface::router>
                    >
                >
            >;

            using InterfaceIndex = NetworkInterfaceTableType::nth_index<0>::type;
            using ChannelIndex = NetworkInterfaceTableType::nth_index<1>::type;
            using RouterIndex = NetworkInterfaceTableType::nth_index<2>::type;

            using InterfaceRange = boost::iterator_range<InterfaceIndex::iterator>;
            using ChannelRange = boost::iterator_range<ChannelIndex::iterator>;
            using RouterRange = boost::iterator_range<RouterIndex::iterator>;

            InterfaceRange getAllInterfaces() const {
                auto& index = mTable.get<0>();
                return std::make_pair(index.begin(), index.end());
            }

            ChannelRange getInterfaceByChannel(Channel channel) const {
                return mTable.get<1>().equal_range(channel);
            }

            RouterRange getInterfaceByRouter(Router* router) const {
                return mTable.get<2>().equal_range(router);
            }

            void insert(Router* router, IDccEntity* entity) {
                auto interface = new NetworkInterface(router, entity);
                mTable.emplace(interface);
            }

        private:
            NetworkInterfaceTableType mTable;
    };

} // namespace artery

#endif
