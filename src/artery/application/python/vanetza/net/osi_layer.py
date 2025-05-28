# Binds: extern/vanetza/vanetza/net/osi_layer.hpp
# pylint: disable=unused-import,function-redefined,useless-parent-delegation

import enum

from typing import List, Iterable, TYPE_CHECKING


class _OsiLayer(enum.Enum):
    Physical = 1
    Link = 2
    Network = 3
    Transport = 4
    Session = 5
    Presentation = 6    
    Application = 7

def _min_osi_layer():
    return _OsiLayer.Physical

def _max_osi_layer():
    return _OsiLayer.Application

def _distance(start: _OsiLayer, end: _OsiLayer) -> int:
    return max(int(start) - int(end), 0)

def _num_osi_layers(start: _OsiLayer, end: _OsiLayer) -> int:
    d = distance(start, end)
    return d + 1 if d > 0 else d


if TYPE_CHECKING:
    OsiLayer = _OsiLayer
    min_osi_layer = _min_osi_layer
    max_osi_layer = _max_osi_layer
    distance = _distance
    num_osi_layers = _num_osi_layers

else:
    try:
        from _vanetza import OsiLayer, min_osi_layer, max_osi_layer
        from _vanetza import distance, num_osi_layers

    except ImportError:
        import warn
    

OsiLayers = [
    OsiLayer.Physical,
    OsiLayer.Link,
    OsiLayer.Network,
    OsiLayer.Transport,
    OsiLayer.Session,
    OsiLayer.Presentation,
    OsiLayer.Application
]

def osi_layer_list(start: OsiLayer, end: OsiLayer) -> List[OsiLayer]:
    return [OsiLayers[layer - 1] for layer in range(int(start), int(end) + 1)]

def osi_layer_iterable(start: OsiLayer, end: OsiLayer) -> Iterable[OsiLayer]:
    for layer in range(int(start), int(end) + 1):
        yield OsiLayers[layer - 1]
