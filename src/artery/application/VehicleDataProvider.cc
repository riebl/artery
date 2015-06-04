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
#include <cassert>
#include <cmath>
#include <random>
#include <boost/math/constants/constants.hpp>
#include <boost/units/cmath.hpp>

vanetza::units::Angle convertOmnetCamAngle(double angle)
{
	assert(angle >= -boost::math::constants::pi<double>());
	assert(angle < boost::math::constants::pi<double>());
	// change rotation ccw -> cw
	// rotate zero from east to north
	// adjust [-pi; pi[ to [0; 360[
	angle /= boost::math::constants::pi<double>();
	angle *= -180.0;
	angle += 450.0;
	angle = std::fmod(angle, 360.0);

	assert(angle >= 0.0);
	assert(angle < 360.0);
	return vanetza::units::Angle::from_value(angle);
}

VehicleDataProvider::VehicleDataProvider() :
		mStationId(rand()), mYawRate(0.0), mCurvature(0.0), mLastUpdate(simTime())
{
}

void VehicleDataProvider::update(const Veins::TraCIMobility* mob)
{
	const double cTimeDelta = (simTime() - mLastUpdate).dbl();
	const double cPi = boost::math::constants::pi<double>();
	if (cTimeDelta == 0.0) {
		mSpeed = mob->getSpeed() * vanetza::units::si::meter_per_second;
		mHeading = convertOmnetCamAngle(mob->getAngleRad());
		mPosition = mob->getCurrentPosition();
		mLastUpdate = simTime();
		return;
	}

	double accel = -mSpeed.value();
	mSpeed = mob->getSpeed() * vanetza::units::si::meter_per_second;
	accel += mSpeed.value();
	accel /= cTimeDelta;
	mAccel = accel * vanetza::units::si::meter_per_second_squared;

	mYawRate = mHeading.value();
	mHeading = convertOmnetCamAngle(mob->getAngleRad());
	mYawRate -= mHeading.value();
	// Correct angle difference when "crossing" 0°/360° border
	if (mYawRate < -180.0) {
		mYawRate += 360.0;
	} else if (mYawRate > 180.0) {
		mYawRate -= 360.0;
	}
	mYawRate /= cTimeDelta;
	// SUMO cars do sometimes very sharp turns causing insane yaw rates
	mYawRate = std::min(180.0, std::max(mYawRate, -180.0));

	std::pair<double, double> posLatLon = mob->getCommandInterface()->getLonLat(mob->getCurrentPosition());
	mLat = posLatLon.first * vanetza::units::degree;
	mLon = posLatLon.second * vanetza::units::degree;

	mLastUpdate = simTime();

	if (std::abs(mYawRate) < 0.1) {
		// yaw rate is pretty small, assume straight road
		mCurvature = 0.0;
	} else if (fabs(mSpeed) < 1.3 * vanetza::units::si::meter_per_second) {
		// less than 5 km/h speed
		mCurvature = 0.0;
	} else {
		// rough curvature estimation
		double dist = mob->getCurrentPosition().distance(mPosition);
		double alpha = (mYawRate * cTimeDelta) / 180.0 * cPi;
		double sin_half_alpha = sin(0.5 * alpha);
		if (sin_half_alpha == 0.0)
			return;
		double radius = (0.5 * dist) / sin_half_alpha;
		if (radius == 0.0)
			return;
		mCurvature = 1.0 / radius;
		assert(std::abs(mCurvature) <= 1.0);
	}

	mPosition = mob->getCurrentPosition();
}

uint16_t VehicleDataProvider::timestamp() const
{
	uint16_t tai = mLastUpdate.inUnit(SIMTIME_MS);
	return tai;
}
