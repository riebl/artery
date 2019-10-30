package artery.inet.gemv2;

module FoliageIndex extends ObstacleIndex
{
    parameters:
        filterTypes = default("forest scrub tree tree_row wood");
        obstacleColor = default("ForestGreen");
        requireFilled = default(true);
}
