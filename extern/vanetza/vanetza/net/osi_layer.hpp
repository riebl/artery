#ifndef OSI_LAYER_HPP_C4VTEZJP
#define OSI_LAYER_HPP_C4VTEZJP

#include <boost/range/iterator_range.hpp>
#include <array>
#include <cstdint>

namespace vanetza
{

enum class OsiLayer : uint8_t
{
    Physical = 1,
    Link = 2,
    Network = 3,
    Transport = 4,
    Session = 5,
    Presentation = 6,
    Application = 7
};

constexpr OsiLayer min_osi_layer() { return OsiLayer::Physical; }
constexpr OsiLayer max_osi_layer() { return OsiLayer::Application; }

constexpr std::array<OsiLayer, 7> osi_layers {{
            OsiLayer::Physical,
            OsiLayer::Link,
            OsiLayer::Network,
            OsiLayer::Transport,
            OsiLayer::Session,
            OsiLayer::Presentation,
            OsiLayer::Application
}};

/**
 * Calculate distance between layers
 * \param from start counting at this layer
 * \param to stop counting here
 * \return 0 if equal layers, positive if "to" is a higher layer, negative if "to" is below "from"
 */
constexpr int distance(OsiLayer from, OsiLayer to)
{
    return static_cast<int>(to) - static_cast<int>(from);
}

constexpr std::size_t num_osi_layers(OsiLayer from, OsiLayer to)
{
    return (from <= to ? distance(from, to) + 1 : 0);
}

template<OsiLayer FROM, OsiLayer TO>
std::array<OsiLayer, num_osi_layers(FROM, TO)> osi_layer_range()
{
    static_assert(FROM <= TO, "FROM layer is above TO layer");
    typedef typename std::underlying_type<OsiLayer>::type num_type;

    num_type num = static_cast<num_type>(FROM);
    std::array<OsiLayer, num_osi_layers(FROM, TO)> layers;
    for (auto& layer : layers) {
        layer = static_cast<OsiLayer>(num++);
    }
    return layers;
}

inline boost::iterator_range<decltype(osi_layers)::const_iterator>
osi_layer_range(OsiLayer from, OsiLayer to)
{
    assert(from <= to);
    auto begin = osi_layers.cbegin() + distance(min_osi_layer(), from);
    auto end = osi_layers.cend() - distance(to, max_osi_layer());
    return boost::make_iterator_range(begin, end);
}

} // namespace vanetza

#endif /* OSI_LAYER_HPP_C4VTEZJP */

