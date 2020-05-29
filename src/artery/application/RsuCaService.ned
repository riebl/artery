//
// Artery V2X Simulation Framework
// Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
//

package artery.application;

simple RsuCaService like ItsG5Service
{
    parameters:
        @signal[CamReceived](type=CaObject);
        @signal[CamSent](type=CaObject);

        @statistic[reception](source=CamReceived;record=vector(camStationId)?,vector(camGenerationDeltaTime)?);
        @statistic[transmission](source=CamSent;record=vector(camStationId)?,vector(camGenerationDeltaTime)?);

        // CAM generation interval
        double generationInterval @unit(s) = default(1.0s);

        // announce protected communication zones (where vehicles need to reduce transmission power)
        xml protectedCommunicationZones = default(xml("<zones/>"));
}
