#ifndef BASICNODEMANAGER_H_XL6ISC2V
#define BASICNODEMANAGER_H_XL6ISC2V

#include "traci/Angle.h"
#include "traci/Boundary.h"
#include "traci/NodeManager.h"
#include "traci/Listener.h"
#include "traci/Position.h"
#include "traci/SubscriptionManager.h"
#include <omnetpp/ccomponent.h>
#include <omnetpp/csimplemodule.h>
#include <functional>
#include <map>
#include <memory>
#include <string>

namespace traci
{

class API;
class ModuleMapper;
class PersonSink;
class VehicleCache;
class VehicleSink;

class BasicNodeManager : public NodeManager, public Listener, public omnetpp::cSimpleModule
{
public:
    static const omnetpp::simsignal_t addNodeSignal;
    static const omnetpp::simsignal_t updateNodeSignal;
    static const omnetpp::simsignal_t removeNodeSignal;
    static const omnetpp::simsignal_t addPersonSignal;
    static const omnetpp::simsignal_t updatePersonSignal;
    static const omnetpp::simsignal_t removePersonSignal;
    static const omnetpp::simsignal_t addVehicleSignal;
    static const omnetpp::simsignal_t updateVehicleSignal;
    static const omnetpp::simsignal_t removeVehicleSignal;

    std::shared_ptr<API> getAPI() override { return m_api; }
    SubscriptionManager* getSubscriptions() { return m_subscriptions; }
    std::size_t getNumberOfNodes() const override;
    std::unordered_set<std::string>& getParkingVehicles();
    std::unordered_set<std::string>& getEndedParkingVehicles();

    /**
     * VehicleObject wraps variable cache of a subscribed TraCI vehicle
     *
     * Each emitted vehicle update signal is accompanied by a VehicleObject (cObject details)
     */
    class VehicleObject : public omnetpp::cObject
    {
    public:
        virtual std::shared_ptr<VehicleCache> getCache() const = 0;
        virtual const TraCIPosition& getPosition() const = 0;
        virtual TraCIAngle getHeading() const = 0;
        virtual double getSpeed() const = 0;
    };

    class PersonObject : public omnetpp::cObject
    {
    public:
        virtual std::shared_ptr<PersonCache> getCache() const = 0;
        virtual const TraCIPosition& getPosition() const = 0;
        virtual TraCIAngle getHeading() const = 0;
        virtual double getSpeed() const = 0;
        virtual std::string getVehicle() const = 0;
    };

protected:
    using NodeInitializer = std::function<void(omnetpp::cModule*)>;

    void initialize() override;
    void finish() override;

    virtual void addPerson(const std::string&);
    virtual void removePerson(const std::string&);
    virtual void updatePerson(const std::string&, PersonSink*);
    virtual void addVehicle(const std::string&);
    virtual void removeVehicle(const std::string&);
    virtual void updateVehicle(const std::string&, VehicleSink*);
    virtual void startVehicleParking(const std::string&);
    virtual void endVehicleParking(const std::string&);
    virtual omnetpp::cModule* createModule(const std::string&, omnetpp::cModuleType*);
    virtual omnetpp::cModule* addNodeModule(const std::string&, omnetpp::cModuleType*, NodeInitializer&);
    virtual void removeNodeModule(const std::string&);
    virtual omnetpp::cModule* getNodeModule(const std::string&);
    virtual PersonSink* getPersonSink(omnetpp::cModule*);
    virtual PersonSink* getPersonSink(const std::string&);
    virtual VehicleSink* getVehicleSink(omnetpp::cModule*);
    virtual VehicleSink* getVehicleSink(const std::string&);
    virtual void processPersons();
    virtual void processVehicles();

    void traciInit() override;
    void traciStep() override;
    void traciClose() override;

private:
    std::shared_ptr<API> m_api;
    ModuleMapper* m_mapper;
    Boundary m_boundary;
    SubscriptionManager* m_subscriptions;
    unsigned m_nodeIndex;
    std::map<std::string, omnetpp::cModule*> m_nodes;
    std::map<std::string, PersonSink*> m_persons;
    std::map<std::string, VehicleSink*> m_vehicles;
    std::string m_vehicle_sink_module;
    std::string m_person_sink_module;
    bool m_destroy_vehicles_on_crash;
    bool m_ignore_persons;
    omnetpp::SimTime m_offset = omnetpp::SimTime::ZERO;

    bool m_handle_parking_vehicles;
    std::unordered_set<std::string> m_parking_vehicles;
    std::unordered_set<std::string> m_ended_parking_vehicles;
    std::unordered_set<std::string> m_removed_vehicles;
};

} // namespace traci

#endif /* BASICNODEMANAGER_H_XL6ISC2V */

