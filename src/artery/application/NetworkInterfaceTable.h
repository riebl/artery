#ifndef ARTERY_NETWORKINTERFACETABLE_H_
#define ARTERY_NETWORKINTERFACETABLE_H_

#include "artery/application/NetworkInterface.h"
#include "artery/utility/Channel.h"
#include <memory>
#include <unordered_set>

namespace artery
{

/**
 * NetworkInterfaceTable maintains NetworkInterfaces registered at a Middleware
 */
class NetworkInterfaceTable
{
    public:
        using TableContainer = std::unordered_set<std::shared_ptr<NetworkInterface>>;

        /**
         * Select the first NetworkInterfaces operating on a particular channel.
         *
         * \param ch NetworkInterface shall operate on this channel
         * \return shared pointer to found NetworkInterface (might be empty)
         */
        std::shared_ptr<NetworkInterface> select(ChannelNumber ch) const;

        /**
         * Get all managed NetworkInterfaces.
         */
        const TableContainer& all() const;

        /**
         * Insert a NetworkInterface
         *
         * \param ifc NetworkInterface instance
         */
        void insert(std::shared_ptr<NetworkInterface> ifc);

    private:
        TableContainer mInterfaces;
};

} // namespace artery

#endif
