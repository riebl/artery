#include <vanetza/facilities/path_history.hpp>
#include <vanetza/units/angle.hpp>
#include <vanetza/units/length.hpp>
#include <boost/units/cmath.hpp>
#include <cassert>

namespace vanetza {
namespace facilities {

const units::Length cAllowableError = 0.47 * units::si::meter;
const units::Length cChordLengthThreshold = 22.5 * units::si::meters;
const units::Length cDistance = 200.0 * units::si::meters;
const units::Length cMaxEstimatedRadius = cREarthMeridian;
const units::Angle cSmallDeltaPhi = units::Angle(1.0 * units::degree);

PathHistory::PathHistory() :
    m_samples(3)
{
}

const PathPoint& PathHistory::starting() const
{
    assert(!m_concise.empty());
    return m_concise.front();
}

const PathPoint& PathHistory::previous() const
{
    assert(m_samples.size() > 1);
    return m_samples[1];
}

const PathPoint& PathHistory::next() const
{
    assert(!m_samples.empty());
    return m_samples.front();
}

void PathHistory::addSample(const PathPoint& point)
{
    m_samples.push_front(point);
    if (m_concise.empty()) {
        m_concise.push_front(m_samples.front());
    }

    updateConcisePoints();
    truncateConcisePoints();
}

const PathPoint& PathHistory::getReferencePoint() const
{
    static const PathPoint scDefaultPathPoint = PathPoint();

    if (m_samples.empty()) {
        return scDefaultPathPoint;
    } else {
        return m_samples.front();
    }
}

void PathHistory::updateConcisePoints()
{
    if (m_samples.full()) {
        const auto actual_chord_length = chord_length(starting(), next());
        units::Length actual_error;
        if (actual_chord_length > cChordLengthThreshold) {
            actual_error = cAllowableError + 1.0 * units::si::meter;
        } else {
            const units::Angle delta_phi = next().heading - starting().heading;
            units::Length estimated_radius;
            if (abs(delta_phi) < cSmallDeltaPhi) {
                actual_error = 0.0 * units::si::meter;
                estimated_radius = cMaxEstimatedRadius;
            } else {
                estimated_radius = actual_chord_length / (2 * sin(delta_phi * 0.5));
                const units::Length d = estimated_radius * cos(0.5 * delta_phi);
                actual_error = estimated_radius - d;
            }
        }

        if (actual_error > cAllowableError) {
            m_concise.push_front(previous());
        }
    }
}

void PathHistory::truncateConcisePoints()
{
    units::Length distance = 0.0 * units::si::meter;
    if (m_concise.size() > 2) {
        auto previous = m_concise.begin();
        auto current = ++m_concise.begin();
        for (; current != m_concise.end(); ++previous, ++current) {
            distance += chord_length(*previous, *current);
            if (distance >= cDistance) {
                m_concise.erase(++current, m_concise.end());
                break;
            }
        }
    }
}

} // namespace facilities
} // namespace vanetza
