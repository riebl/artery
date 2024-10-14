//
// Artery V2X Simulation Framework
// Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
//

package artery.application.cp;


// CpService core implementation
simple Core
{
    parameters:
        @class(CpService);

        @signal[CpmReceived](type=CpObject);
        @signal[CpmSent](type=CpObject);


        @signal[CpmSent_positionX](type=double);
        @statistic[CpmSent_positionX](title="CPM: X-Position while sending CPM segment"; source="CpmSent_positionX"; record=vector; interpolationmode=none);

        @signal[CpmSent_positionY](type=double);
        @statistic[CpmSent_positionY](title="CPM: Y-Position while sending CPM segment"; source="CpmSent_positionY"; record=vector; interpolationmode=none);

        @signal[CpmSent_sender](type=unsigned long);
        @statistic[CpmSent_sender](title="CPM: Sender of sent CPM"; source="CpmSent_sender"; record=vector?; interpolationmode=none);

        @signal[CpmSent_generatedPocCount](type=unsigned long);
        @statistic[CpmSent_generatedPocCount](title="CPM: Number of perceived objects selected for CPM inclusion by generation rule"; source="CpmSent_generatedPocCount"; record=vector; interpolationmode=none);

        @signal[CpmSent_mitigatedPocCount](type=unsigned long);
        @statistic[CpmSent_mitigatedPocCount](title="CPM: Number of perceived objects selected for CPM inclusion after redundancy mitigation"; source="CpmSent_mitigatedPocCount"; record=vector; interpolationmode=none);

        @signal[CpmSent_channelBusyRatio](type=double);
        @statistic[CpmSent_channelBusyRatio](title="CPM: Channel Busy Ratio at the sender"; source="CpmSent_channelBusyRatio"; record=vector; interpolationmode=none);

        @signal[CpmSent_segmentCount](type=unsigned long);
        @statistic[CpmSent_segmentCount](title="CPM: Number of generated CPM segments"; source="CpmSent_segmentCount"; record=vector; interpolationmode=none);

        @signal[CpmSent_pocIncluded](type=bool);
        @statistic[CpmSent_pocIncluded](title="CPM: Perceived Objects Container included flag"; source="CpmSent_pocIncluded"; record=vector?; interpolationmode=none);

        @signal[CpmSent_poCountSegment](type=unsigned long);
        @statistic[CpmSent_poCountSegment](title="CPM: Number of POs included in sent CPM segment"; source="CpmSent_poCountSegment"; record=vector; interpolationmode=none);

        @signal[CpmSent_sicIncludedInSegmentNr](type=unsigned long);
        @statistic[CpmSent_sicIncludedInSegmentNr](title="CPM: Sensor Information Container included flag"; source="CpmSent_sicIncludedInSegmentNr"; record=vector?; interpolationmode=none);

        @signal[CpmSent_msgSize](type=unsigned long);
        @statistic[CpmSent_msgSize](title="CPM: Size of sent segment"; source="CpmSent_msgSize"; record=vector; interpolationmode=none);


        @signal[CpService_10Hz_numSeenByRadar](type=unsigned long);
        @statistic[CpService_10Hz_numSeenByRadar](title="CPM: Number of radar objects currently in the LEM during"; source="CpService_10Hz_numSeenByRadar"; record=vector; interpolationmode=none);

        @signal[CpService_10Hz_numSeenByCam](type=unsigned long);
        @statistic[CpService_10Hz_numSeenByCam](title="CPM: Number of CAM objects currently in the LEM during"; source="CpService_10Hz_numSeenByCam"; record=vector; interpolationmode=none);

        @signal[CpService_10Hz_numSeenByCpm](type=unsigned long);
        @statistic[CpService_10Hz_numSeenByCpm](title="CPM: Number of CPM objects currently known"; source="CpService_10Hz_numSeenByCpm"; record=vector; interpolationmode=none);

        @signal[CpService_10Hz_numSeenByAll](type=unsigned long);
        @statistic[CpService_10Hz_numSeenByAll](title="CPM: Number of unique objects that are simultaneously seen by radar, CAM and CPM"; source="CpService_10Hz_numSeenByAll"; record=vector; interpolationmode=none);


        @signal[CpmRecv_positionX](type=double);
        @statistic[CpmRecv_positionX](title="CPM: X-Position while receiving CPM segment"; source="CpmRecv_positionX"; record=vector; interpolationmode=none);

        @signal[CpmRecv_positionY](type=double);
        @statistic[CpmRecv_positionY](title="CPM: Y-Position while receiving CPM segment"; source="CpmRecv_positionY"; record=vector; interpolationmode=none);

        @signal[CpmRecv_sender](type=unsigned long);
        @statistic[CpmRecv_sender](title="CPM: Sender of received CPM"; source="CpmRecv_sender"; record=vector?; interpolationmode=none);

        @signal[CpmRecv_delay](type=long);
        @statistic[CpmRecv_delay](title="Communication delay between sending and receiving CPM, i.e., beetween request() and indicate()"; source="CpmRecv_delay"; record=vector?; interpolationmode=none);

        @signal[CpmRecv_distToSender](type=double);
        @statistic[CpmRecv_distToSender](title="CPM: Distance from receiver to sender of this CPM"; source="CpmRecv_distToSender"; record=vector?; interpolationmode=none);

        @signal[CpmRecv_objId](type=unsigned long);
        @statistic[CpmRecv_objId](title="CPM: IDs of received PO"; source="CpmRecv_objId"; record=vector?; interpolationmode=none);

        @signal[CpmRecv_age](type=long);
        @statistic[CpmRecv_age](title="CPM: Overall age of received PO"; source="CpmRecv_age"; record=vector; interpolationmode=none);

        @signal[CpmRecv_distance](type=double);
        @statistic[CpmRecv_distance](title="CPM: Distance to received PO"; source="CpmRecv_distance"; record=vector; interpolationmode=none);

        @signal[CpmRecv_poCountSegment](type=unsigned long);
        @statistic[CpmRecv_poCountSegment](title="CPM: Number of POs included in received CPM segment"; source="CpmRecv_poCountSegment"; record=vector; interpolationmode=none);

        @signal[CpmRecv_poTimeSinceLastUpdate](type=long);
        @statistic[CpmRecv_poTimeSinceLastUpdate](title="CPM: Time passed since information about this PO was received last"; source="CpmRecv_poTimeSinceLastUpdate"; record=vector; interpolationmode=none);

        @signal[CpmRecv_poDistanceSinceLastUpdate](type=double);
        @statistic[CpmRecv_poDistanceSinceLastUpdate](title="CPM: Distance the PO traveled between is was included in this and the last CPM"; source="CpmRecv_poDistanceSinceLastUpdate"; record=vector; interpolationmode=none);

        @signal[CpmRecv_poRedundancy](type=unsigned long);
        @statistic[CpmRecv_poRedundancy](title="CPM: Number of CPMs received in the validity period including information about this object"; source="CpmRecv_poRedundancy"; record=vector; interpolationmode=none);

        @signal[CpmRecv_senderRedundancy](type=unsigned long);
        @statistic[CpmRecv_senderRedundancy](title="CPM: Number of senders in the validity period that sent information about this object"; source="CpmRecv_senderRedundancy"; record=vector; interpolationmode=none);

        @signal[CpmRecv_senderHistoryOverflow](type=unsigned long);
        @statistic[CpmRecv_senderHistoryOverflow](title="CPM: ID of sender that caused a history buffer sourced by sender overflow in the validity period"; source="CpmRecv_senderHistoryOverflow"; record=vector?; interpolationmode=none);

        @signal[CpmRecv_objectHistoryOverflow](type=unsigned long);
        @statistic[CpmRecv_objectHistoryOverflow](title="CPM: ID of sender that caused a history buffer sourced by object overflow in the validity period"; source="CpmRecv_objectHistoryOverflow"; record=vector?; interpolationmode=none);

        @signal[CpmRecv_senderDupeDrop](type=unsigned long);
        @statistic[CpmRecv_senderDupeDrop](title="CPM: ID of sender that sent a duplicate CPM that got dropped"; source="CpmRecv_senderDupeDrop"; record=vector?; interpolationmode=none);

        @signal[CpmRecv_objectDupeDrop](type=unsigned long);
        @statistic[CpmRecv_objectDupeDrop](title="CPM: ID of object that is part of a sent duplicate CPM that got dropped"; source="CpmRecv_objectDupeDrop"; record=vector?; interpolationmode=none);

        @signal[CpmRecv_senderDupeReplace](type=unsigned long);
        @statistic[CpmRecv_senderDupeReplace](title="CPM: ID of sender that sent a duplicate CPM that replaced the previous CPM"; source="CpmRecv_senderDupeReplace"; record=vector?; interpolationmode=none);

        @signal[CpmRecv_objectDupeReplace](type=unsigned long);
        @statistic[CpmRecv_objectDupeReplace](title="CPM: ID of object that is part of a sent duplicate CPM that replaced the previous CPM"; source="CpmRecv_objectDupeReplace"; record=vector?; interpolationmode=none);


        // Minimum interval for CPM generation
        int genCpmMin @unit(ms) = default(100ms);
        // Maximum interval for CPM generation
        int genCpmMax @unit(ms) = default(1000ms);
        // Interval for SensorInformationContainer inclusion
        int addSensorInformation @unit(ms) = default(1000ms);

        // Maximum CPM size
        int mtuCpm @unit(byte) = default(1394byte);
        // DCC Profile to use
        // The numbers 0 - 3 map to DP0 - DP3
        int dccProfile @enum(0, 1, 2, 3) = default(2);
        // Apply DCC transmission interval restrictions
        bool withDccRestriction = default(false);

        // If true enable redundancy mitigation dynamically, otherwise ignore redundancy parameters and enable always
        bool dynamicRedundancyMitigation = default(false);
        // Network channel load (Channel Busy Ratio) that needs to get exceeded to use redundancy mitigation
        double redundancyLoad = default(0.25);
}
