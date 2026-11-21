#pragma once

#include <artery/sionna/environment/api/SionnaAPI.h>
#include <boost/bimap.hpp>

#include <string>

namespace artery::sionna
{

// Converts IDs between SUMO and Sionna scene naming rules.
class TraciIDConverterProxy : public IIDConverterProxy
{
public:
    // IIDConverterProxy implementation.
    std::string convertID(IDNamespace from, IDNamespace to, const std::string& id) override;
    void removeID(IDNamespace ns, const std::string& id) override;

    // Remove cached mapping by SUMO ID.
    void removeBySumoID(const std::string& id);
    // Remove cached mapping by Sionna ID.
    void removeBySionnaID(const std::string& id);

private:
    std::string sumoID(const std::string& sionnaID) const;
    std::string sionnaID(const std::string& sumoID);

private:
    // left is SUMO IDs, right is Sionna IDs.
    boost::bimap<std::string, std::string> mapping_;
};

}  // namespace artery::sionna
