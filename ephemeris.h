#pragma once

#include <QVector3D>
#include <QDateTime>

#include "config.h"

struct PlanetImpl
{
	PlanetImpl(PlanetConfig::Config const &c);
	QVector3D getPosition(QDateTime const &date);
	double getRotationAngle(QDateTime const &time);

	struct Orbit
	{
		double a; //! semi-major axis
		double e; //! eccentricity
		double i; //! inclination
		double w; //! argument of periapsis
		double W; //! logtitude of the ascending node
		double l; //! mean longtituude
	};

    Orbit _initial_orbit;
    Orbit _delta_orbit;

	double _radius;
	double _rotation_period;
	QVector3D getEllipsePos(Orbit const &eph, double m);
    Orbit getEphemeris(double date);
	double toJulianDay(QDateTime const &date);
	double getEphemerisValue(double jdn, double initial, double rate_per_century);
};

