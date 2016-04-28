#include "traffic_class.hpp"
#include "serialization.hpp"

namespace vanetza
{
namespace geonet
{

TrafficClass::TrafficClass() : m_tc(0)
{
}

TrafficClass::TrafficClass(bool scf, bool ch_offload, BitNumber<unsigned, 6> tc) : m_tc(0)
{
    store_carry_forward(scf);
    channel_offload(ch_offload);
    tc_id(tc);
}

TrafficClass::TrafficClass(uint8_t raw) : m_tc(raw)
{
}

bool TrafficClass::store_carry_forward() const
{
    return (m_tc & scf_mask);
}

void TrafficClass::store_carry_forward(bool flag)
{
    if (flag) {
        m_tc |= scf_mask;
    } else {
        m_tc &= ~scf_mask;
    }
}

bool TrafficClass::channel_offload() const
{
    return (m_tc & channel_offload_mask);
}

void TrafficClass::channel_offload(bool flag)
{
    if (flag) {
        m_tc |= channel_offload_mask;
    } else {
        m_tc &= ~channel_offload_mask;
    }
}

BitNumber<unsigned, 6> TrafficClass::tc_id() const
{
    return (m_tc & tc_id_mask);
}

void TrafficClass::tc_id(BitNumber<unsigned, 6> id)
{
    m_tc &= ~tc_id_mask;
    m_tc |= id.raw() & tc_id_mask;
}

dcc::Profile map_tc_onto_profile(const TrafficClass& tc)
{
    dcc::Profile profile = dcc::Profile::DP3;

    switch (tc.tc_id().raw()) {
        case 0:
            profile = dcc::Profile::DP0;
            break;
        case 1:
            profile = dcc::Profile::DP1;
            break;
        case 2:
            profile = dcc::Profile::DP2;
            break;
        default:
            profile = dcc::Profile::DP3;
            break;
    }

    return profile;
}

void serialize(const TrafficClass& tc, OutputArchive& ar)
{
    serialize(host_cast(tc.raw()), ar);
}

void deserialize(TrafficClass& tc, InputArchive& ar)
{
    decltype(tc.raw()) tmp;
    deserialize(tmp, ar);
    tc = TrafficClass(tmp);
}

} // namespace geonet
} // namespace vanetza

