packet BlackIceReport
{
    double positionX;
    double positionY;
    double speed;
    simtime_t time;
}

packet BlackIceQuery
{
    double positionX;
    double positionY;
    double radius;
}

packet BlackIceResponse
{
    int warnings;
}
