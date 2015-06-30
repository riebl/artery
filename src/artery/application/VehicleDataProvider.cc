//
// Copyright (C) 2014 Raphael Riebl <raphael.riebl@thi.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "artery/application/VehicleDataProvider.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#undef ev
#include <boost/math/constants/constants.hpp>
#include <boost/units/cmath.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <vanetza/units/frequency.hpp>
#include <vanetza/units/time.hpp>
#include <vanetza/units/angle.hpp>
#include <vanetza/units/angular_velocity.hpp>
#include <cassert>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

const double pi = boost::math::constants::pi<double>();
const auto degree_per_second_squared = vanetza::units::degree / (vanetza::units::si::second * vanetza::units::si::second);
const std::map<VehicleDataProvider::AngularAcceleration, double> VehicleDataProvider::mConfidenceTable {
    { AngularAcceleration(0.0 * degree_per_second_squared), 1.0 },
    { AngularAcceleration(0.5 * degree_per_second_squared), 0.9 },
    { AngularAcceleration(1.0 * degree_per_second_squared), 0.8 },
    { AngularAcceleration(1.5 * degree_per_second_squared), 0.7 },
    { AngularAcceleration(2.0 * degree_per_second_squared), 0.6 },
    { AngularAcceleration(2.5 * degree_per_second_squared), 0.5 },
    { AngularAcceleration(5.0 * degree_per_second_squared), 0.4 },
    { AngularAcceleration(10.0 * degree_per_second_squared), 0.3 },
    { AngularAcceleration(15.0 * degree_per_second_squared), 0.2 },
    { AngularAcceleration(20.0 * degree_per_second_squared), 0.1 },
    { AngularAcceleration(25.0 * degree_per_second_squared), 0.0 },
    { AngularAcceleration(std::numeric_limits<double>::infinity() * degree_per_second_squared), 0.0 }
};

vanetza::units::Angle convertMobilityAngle(vanetza::units::Angle angle)
{
	using vanetza::units::si::radians;
	assert(angle >= -pi * radians);
	assert(angle < pi * radians);
	// change rotation ccw -> cw
	angle *= -1.0;
	// rotate zero from east to north
	// adjust [-pi; pi[ to [0; 2pi[
	angle += 2.5 * pi * radians;
	angle = boost::units::fmod(angle, 2.0 * pi * radians);

	assert(angle >= 0.0 * radians);
	assert(angle < 2.0 * pi * radians);
	return angle;
}

VehicleDataProvider::VehicleDataProvider() :
	mStationId(rand()), mConfidence(0.0), mLastUpdate(simTime()),
	mCurvatureOutput(2), mCurvatureConfidenceOutput(2)
{
	while (!mCurvatureConfidenceOutput.full()) {
		using namespace vanetza::units::si;
		mCurvatureConfidenceOutput.push_front(0.0 * radians_per_second / second);
	}
}

void VehicleDataProvider::calculateCurvature()
{
	using namespace vanetza::units::si;
	static const vanetza::units::Frequency f_cut = 0.33 * hertz;
	static const vanetza::units::Duration t_sample = 100.0 * seconds;
	static const vanetza::units::Curvature lower_threshold = 1.0 / 2500.0 * vanetza::units::reciprocal_metre;
	static const vanetza::units::Curvature upper_threshold = 1.0 * vanetza::units::reciprocal_metre;
	static const double damping = 1.0;

	if (fabs(mSpeed) < 1.0 * meter_per_second) {
		// assume straight road below minimum speed
		mCurvature = 0.0 * vanetza::units::reciprocal_metre;
	} else {
		// curvature calculation algorithm
		mCurvature = (mYawRate / radians) / mSpeed;

		if (!mCurvatureOutput.full()) {
			// save first two values for initialization
			mCurvatureOutput.push_front(mCurvature);
			mCurvature = 0.0 * vanetza::units::reciprocal_metre;
		} else {
			static const auto omega = 2.0 * pi * f_cut;
			mCurvature = - mCurvatureOutput[1] +
				(2.0 + 2.0 * omega * damping * t_sample) * mCurvatureOutput[0] +
				omega * omega * t_sample * t_sample * mCurvature;
			mCurvature /= 1.0 + 2.0 * omega * damping * t_sample + omega * omega * t_sample * t_sample;
			mCurvatureOutput.push_front(mCurvature);

			// assume straight road below threshold
			if (fabs(mCurvature) < lower_threshold) {
				mCurvature = 0.0 * vanetza::units::reciprocal_metre;
			} else if (fabs(mCurvature) > upper_threshold) {
				// clamp minimum radius to 1 meter
				mCurvature = upper_threshold;
			}
		}
	}
}

void VehicleDataProvider::calculateCurvatureConfidence()
{
	assert(mCurvatureConfidenceOutput.full());
	using namespace vanetza::units::si;
	static const vanetza::units::Frequency f_cut = 1.0 * hertz;
	static const vanetza::units::Duration t_sample = 100.0 * seconds;
	static const double damping = 1.0;
	static const auto omega = 2.0 * pi * f_cut;

	AngularAcceleration filter = -mCurvatureConfidenceOutput[1] +
		(2.0 + 2.0 * omega * damping * t_sample) * mCurvatureConfidenceOutput[0] +
		omega * omega * t_sample * mYawRate -
		omega * omega * t_sample * mCurvatureConfidenceInput;
	filter /= 1.0 + 2.0 * omega * damping * t_sample + omega * omega * t_sample * t_sample;
	mCurvatureConfidenceOutput.push_front(filter);
	mCurvatureConfidenceInput = mYawRate;
	mConfidence = mapOntoConfidence(abs(filter));
}

void VehicleDataProvider::update(const Veins::TraCIMobility* mob)
{
	using namespace vanetza::units::si;
	using boost::units::si::milli;
	const vanetza::units::Duration delta {
		(simTime() - mLastUpdate).inUnit(SIMTIME_MS) * milli * seconds
	};

	if (delta <= 0.0 * seconds) {
		mSpeed = mob->getSpeed() * meter_per_second;
		mHeading = convertMobilityAngle(mob->getAngleRad() * radians);
	} else {
		using boost::units::abs;
		auto new_speed = mob->getSpeed() * meter_per_second;
		mAccel = (new_speed - mSpeed) / delta;
		mSpeed = new_speed;

		auto new_heading = convertMobilityAngle(mob->getAngleRad() * radians);
		auto diff_heading = pi * radians - abs(abs(new_heading - mHeading) - pi * radians);
		mYawRate = diff_heading / delta;
		mHeading = new_heading;
	}

	mPosition = mob->getCurrentPosition();
	std::pair<double, double> posLatLon = mob->getGeoPosition();
	mLat = posLatLon.first * vanetza::units::degree;
	mLon = posLatLon.second * vanetza::units::degree;
	mLastUpdate = simTime();

	calculateCurvature();
	calculateCurvatureConfidence();
}

double VehicleDataProvider::mapOntoConfidence(AngularAcceleration x) const
{
	auto it = mConfidenceTable.lower_bound(x);
	if (it == mConfidenceTable.end()) {
		throw std::domain_error("input value is less than smallest entry in confidence table");
	}
	return it->second;
}

uint16_t VehicleDataProvider::timestamp() const
{
	uint16_t tai = mLastUpdate.inUnit(SIMTIME_MS);
	return tai;
}
