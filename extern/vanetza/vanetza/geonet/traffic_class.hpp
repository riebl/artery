#ifndef TRAFFIC_CLASS_HPP_I2WYKREX
#define TRAFFIC_CLASS_HPP_I2WYKREX

#include <vanetza/common/bit_number.hpp>
#include <vanetza/dcc/profile.hpp>
#include <vanetza/geonet/serialization.hpp>
#include <cstdint>

namespace vanetza
{

namespace geonet
{

class TrafficClass
{
public:
    TrafficClass();
    TrafficClass(bool scf, bool channel_offload, BitNumber<unsigned, 6> tc_id);
    explicit TrafficClass(uint8_t raw);

    bool store_carry_forward() const;
    void store_carry_forward(bool flag);
    bool channel_offload() const;
    void channel_offload(bool flag);
    BitNumber<unsigned, 6> tc_id() const;
    void tc_id(BitNumber<unsigned, 6> id);
    uint8_t raw() const { return m_tc; }

private:
    static constexpr uint8_t scf_mask = 0x80;
    static constexpr uint8_t channel_offload_mask = 0x40;
    static constexpr uint8_t tc_id_mask = 0x3f;
    uint8_t m_tc;
};

dcc::Profile map_tc_onto_profile(const TrafficClass&);

void serialize(const TrafficClass&, OutputArchive&);
void deserialize(TrafficClass&, InputArchive&);

} // namespace geonet
} // namespace vanetza

#endif /* TRAFFIC_CLASS_HPP_I2WYKREX */

