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

#ifndef VEHICLEDATAPROVIDER_H_
#define VEHICLEDATAPROVIDER_H_

#include "veins/base/utils/Coord.h"
#include <simtime_t.h>
#include <boost/circular_buffer.hpp>
#include <boost/units/systems/si/angular_acceleration.hpp>
#include <vanetza/units/acceleration.hpp>
#include <vanetza/units/angle.hpp>
#include <vanetza/units/velocity.hpp>
#include <vanetza/units/angular_velocity.hpp>
#include <vanetza/units/curvature.hpp>
#include <cstdint>
#include <map>

namespace Veins { class TraCIMobility; }

class VehicleDataProvider
{
	public:
		VehicleDataProvider();
		void update(const Veins::TraCIMobility*);

		uint32_t station_id() const { return mStationId; }
		uint16_t timestamp() const; // ms TAI timestamp
		const simtime_t& simtime() const { return mLastUpdate; }
		const Coord& position() const { return mPosition; }
		vanetza::units::GeoAngle longitude() const { return mLon; } // positive for east
		vanetza::units::GeoAngle latitude() const { return mLat; } // positive for north
		vanetza::units::Velocity speed() const { return mSpeed; }
		vanetza::units::Acceleration acceleration() const { return mAccel; }
		vanetza::units::Angle heading() const { return mHeading; } // degree from north, clockwise
		vanetza::units::AngularVelocity yaw_rate() const { return mYawRate; } // counter-clockwise positive
		vanetza::units::Curvature curvature() const { return mCurvature; } // 1/m radius, left turn positive
		double confidence() const { return mConfidence; } // percentage value

	private:
		typedef boost::units::quantity<boost::units::si::angular_acceleration> AngularAcceleration;
		void calculateCurvature();
		void calculateCurvatureConfidence();
		double mapOntoConfidence(AngularAcceleration) const;

		uint32_t mStationId;
		vanetza::units::GeoAngle mLon;
		vanetza::units::GeoAngle mLat;
		vanetza::units::Velocity mSpeed;
		vanetza::units::Acceleration mAccel;
		vanetza::units::Angle mHeading;
		vanetza::units::AngularVelocity mYawRate;
		vanetza::units::Curvature mCurvature;
		double mConfidence;
		Coord mPosition;
		simtime_t mLastUpdate;
		boost::circular_buffer<vanetza::units::Curvature> mCurvatureOutput;
		boost::circular_buffer<AngularAcceleration> mCurvatureConfidenceOutput;
		vanetza::units::AngularVelocity mCurvatureConfidenceInput;
		static const std::map<AngularAcceleration, double> mConfidenceTable;
};

#endif /* VEHICLEDATAPROVIDER_H_ */
