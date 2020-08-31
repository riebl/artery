package artery.application;

simple PeriodicLoadService like ItsG5Service
{
    parameters:
        bool waitForFirstTrigger = default(true);
        volatile double generationInterval @unit(s) = default(1s);
        volatile int payloadLength @unit(byte) = default(300B);
}
