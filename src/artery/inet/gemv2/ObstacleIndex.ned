package artery.inet.gemv2;

simple ObstacleIndex
{
    parameters:
        @class(artery::gemv2::ObstacleIndex);
        string traciModule;
        string filterTypes = default("building shop amenity landuse forest natural wood tree tree_row");
}
