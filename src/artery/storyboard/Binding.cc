#include "artery/storyboard/AndCondition.h"
#include "artery/storyboard/CarSetCondition.h"
#include "artery/storyboard/DeferringCondition.h"
#include "artery/storyboard/EmergencyStopEffect.h"
#include "artery/storyboard/GenericEffectFactory.h"
#include "artery/storyboard/SignalEffectFactory.h"
#include "artery/storyboard/SpeedCondition.h"
#include "artery/storyboard/SpeedDifferenceCondition.h"
#include "artery/storyboard/SpeedEffectFactory.h"
#include "artery/storyboard/StopEffectFactory.h"
#include "artery/storyboard/Story.h"
#include "artery/storyboard/Storyboard.h"
#include "artery/storyboard/LikelihoodCondition.h"
#include "artery/storyboard/LimitCondition.h"
#include "artery/storyboard/OrCondition.h"
#include "artery/storyboard/PolygonCondition.h"
#include "artery/storyboard/TimeCondition.h"
#include "artery/storyboard/TtcCondition.h"
#include "artery/utility/Geometry.h"
#include <omnetpp/simtime.h>
#include <pybind11/embed.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace artery
{

PYBIND11_EMBEDDED_MODULE(storyboard, m) {

    py::class_<Position>(m, "Coord")
        .def(py::init<double, double>());

    py::class_<omnetpp::SimTime>(m, "SimTime")
        .def(py::init<double>())
        .def(py::self + py::self)
        .def(py::self - py::self);

    py::class_<omnetpp::cRNG>(m, "RNG");

    py::class_<Condition, std::shared_ptr<Condition>>(m, "Condition");

    py::class_<AndCondition, std::shared_ptr<AndCondition>, Condition>(m, "AndCondition")
        .def(py::init<std::shared_ptr<Condition>, std::shared_ptr<Condition>>());

    py::class_<OrCondition, std::shared_ptr<OrCondition>, Condition>(m, "OrCondition")
        .def(py::init<std::shared_ptr<Condition>, std::shared_ptr<Condition>>());

    py::class_<CarSetCondition, std::shared_ptr<CarSetCondition>, Condition>(m, "CarSetCondition")
        .def(py::init<std::string>())
        .def(py::init<std::set<std::string>>());

    py::class_<DeferringCondition, std::shared_ptr<DeferringCondition>, Condition>(m, "DeferringCondition")
        .def(py::init<omnetpp::cRNG*, omnetpp::SimTime, omnetpp::SimTime>());

    py::class_<LikelihoodCondition, std::shared_ptr<LikelihoodCondition>, Condition>(m, "LikelihoodCondition")
        .def(py::init<omnetpp::cRNG*, double>());

    py::class_<LimitCondition, std::shared_ptr<LimitCondition>, Condition>(m, "LimitCondition")
        .def(py::init<unsigned>());

    py::class_<PolygonCondition, std::shared_ptr<PolygonCondition>, Condition>(m, "PolygonCondition")
        .def(py::init<std::vector<Position>>());

    using SpeedConditionLess = SpeedCondition<std::less<vanetza::units::Velocity>>;
    py::class_<SpeedConditionLess, std::shared_ptr<SpeedConditionLess>, Condition>(m, "SpeedConditionLess")
        .def(py::init<double>());

    using SpeedConditionGreater = SpeedCondition<std::greater<vanetza::units::Velocity>>;
    py::class_<SpeedConditionGreater, std::shared_ptr<SpeedConditionGreater>, Condition>(m, "SpeedConditionGreater")
        .def(py::init<double>());

    py::class_<SpeedDifferenceConditionFaster, std::shared_ptr<SpeedDifferenceConditionFaster>, Condition>
        (m, "SpeedDifferenceConditionFaster")
        .def(py::init<double>());

    py::class_<SpeedDifferenceConditionSlower, std::shared_ptr<SpeedDifferenceConditionSlower>, Condition>
        (m, "SpeedDifferenceConditionSlower")
        .def(py::init<double>());

    py::class_<TimeCondition, std::shared_ptr<TimeCondition>, Condition>(m, "TimeCondition")
        .def(py::init<omnetpp::SimTime>())
        .def(py::init<omnetpp::SimTime, omnetpp::SimTime>())
        .def(py::init<omnetpp::SimTime>(), py::arg("begin"))
        .def(py::init<omnetpp::SimTime, omnetpp::SimTime>(), py::arg("begin"), py::arg("end"));

    py::class_<TtcCondition, std::shared_ptr<TtcCondition>, Condition>(m, "TtcCondition")
        .def(py::init<double, double>());


    py::class_<EffectFactory, std::shared_ptr<EffectFactory>>(m, "EffectFactory");

    py::class_<GenericEffectFactory, std::shared_ptr<GenericEffectFactory>, EffectFactory>(m, "EmergencyStopEffect")
        .def(py::init([]() -> GenericEffectFactory* {
                return new GenericEffectFactory {
                    [](Vehicle& v, Story& s, ConditionResult&) { return std::make_shared<EmergencyStopEffect>(s, v); }
                };
            }));

    py::class_<SpeedEffectFactory, std::shared_ptr<SpeedEffectFactory>, EffectFactory>(m, "SpeedEffect")
        .def(py::init<double>());

    py::class_<StopEffectFactory, std::shared_ptr<StopEffectFactory>, EffectFactory>(m, "StopEffect")
        .def(py::init<>());

    py::class_<SignalEffectFactory, std::shared_ptr<SignalEffectFactory>, EffectFactory>(m, "SignalEffect")
        .def(py::init<std::string>());


    py::class_<Story, std::shared_ptr<Story>>(m, "Story")
        .def(py::init<std::shared_ptr<Condition>, std::vector<std::shared_ptr<EffectFactory>>>());

    py::class_<artery::Storyboard>(m, "Storyboard")
        .def("registerStory", &artery::Storyboard::registerStory)
        .def("getWarmupEndTime", [](artery::Storyboard& board) { return board.getSimulation()->getWarmupPeriod(); })
        .def("convertTraciPosition", &artery::Storyboard::convertTraciPosition)
        .def("getRandomNumberGenerator", &artery::Storyboard::getRNG, py::return_value_policy::reference);
}

/**
 * Wrapper function for creating a SimTime in seconds
 * \param seconds
 * \return SimTime, unit: seconds
 */
omnetpp::SimTime timelineSeconds(double time) {
    return omnetpp::SimTime { time };
}

omnetpp::SimTime timelineSeconds(std::int64_t time) {
    return omnetpp::SimTime { time, omnetpp::SIMTIME_S };
}

/**
 * Wrapper function for creating a SimTime in milliseconds
 * \param milliseconds
 * \return SimTime, unit: milliseconds
 */
omnetpp::SimTime timelineMilliseconds(double time) {
    return omnetpp::SimTime { time * 1e-3 };
}

omnetpp::SimTime timelineMilliseconds(std::int64_t time) {
    return omnetpp::SimTime { time, omnetpp::SIMTIME_MS };
}

template<typename... Args>
using overload_cast = pybind11::detail::overload_cast_impl<Args...>;

PYBIND11_EMBEDDED_MODULE(timeline, m) {
    m.def("seconds", overload_cast<std::int64_t>()(&timelineSeconds));
    m.def("seconds", overload_cast<double>()(&timelineSeconds));
    m.def("milliseconds", overload_cast<std::int64_t>()(&timelineMilliseconds));
    m.def("milliseconds", overload_cast<double>()(&timelineMilliseconds));
}

} // namespace artery

