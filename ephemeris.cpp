#include <cmath>

#include "ephemeris.h"

PlanetImpl::PlanetImpl(PlanetConfig::Config const &c)
{
	_rotation_period = c.initial_rot_period;
	_radius = c.initial_inner_rad / 149597870.691;
	_initial_orbit.a = c.initial_sm_axis;
	_initial_orbit.e = c.initial_ecc;
	_initial_orbit.i = c.initial_incl;
	_initial_orbit.w = c.initial_per_arg;
	_initial_orbit.W = c.initial_an_long;
	_initial_orbit.l = c.initial_mean_long;

	_delta_orbit.a = c.delta_sm_axis;
	_delta_orbit.e = c.delta_ecc;
	_delta_orbit.i = c.delta_incl;
	_delta_orbit.w = c.delta_per_arg;
	_delta_orbit.W = c.delta_an_long;
	_delta_orbit.l = c.delta_mean_long;
}

QVector3D PlanetImpl::getEllipsePos(const Orbit &eph, double M)
{
	double w = eph.w - eph.W;
	double I = eph.i;
	double E = M;
	double oE = M + 1.0;
	while (std::abs(E - oE) > 1e-10) {
		oE = E;
		E = M + eph.e * std::sin(E);
	}
	double x = eph.a * (std::cos(E) - eph.e);
	double y = eph.a * std::sqrt(1 - eph.e * eph.e) * std::sin(E);
	double sO = std::sin(eph.W);
	double cO = std::cos(eph.W);
	double sw = std::sin(w);
	double cw = std::cos(w);
	double cc = cw * cO;
	double ss = sw * sO;
	double sc = sw * cO;
	double cs = cw * sO;
	double ci = std::cos(I);
	double si = std::sin(I);
	double X  = (cc - ss * ci) * x + (-sc - cs * ci) * y;
	double Y  = (std::sin(w) * si) * x + (std::cos(w) * si) * y;
	double Z  = -(cs + sc * ci) * x - (-ss + cc * ci) * y;
    return QVector3D(X, Y, Z);
}

double PlanetImpl::getEphemerisValue(double jdn, double initial, double rate_per_century)
{
	double centuries = (jdn - 2451543.5) / 36525.0;
	return initial + rate_per_century * centuries;
}

PlanetImpl::Orbit PlanetImpl::getEphemeris(double date)
{
	Orbit eph;
	eph.a = getEphemerisValue(date, _initial_orbit.a, _delta_orbit.a);
	eph.e = getEphemerisValue(date, _initial_orbit.e, _delta_orbit.e);
	eph.i = getEphemerisValue(date, M_PI / 180.0 * _initial_orbit.i, M_PI / 180.0 * _delta_orbit.i);
	eph.l = getEphemerisValue(date, M_PI / 180.0 * _initial_orbit.l, M_PI / 180.0 * _delta_orbit.l);
	eph.w = getEphemerisValue(date, M_PI / 180.0 * _initial_orbit.w, M_PI / 180.0 * _delta_orbit.w);
	eph.W = getEphemerisValue(date, M_PI / 180.0 * _initial_orbit.W, M_PI / 180.0 * _delta_orbit.W);
	return eph;
}

double PlanetImpl::toJulianDay(const QDateTime &date)
{
	return static_cast<double>(date.date().toJulianDay())
			+ date.time().hour()   / (24.0)
			+ date.time().minute() / (24.0 * 60.0)
			+ date.time().second() / (24.0 * 60.0 * 60.0)
			+ date.time().msec()   / (24.0 * 60.0 * 60.0 * 1000.0);
}

QVector3D PlanetImpl::getPosition(const QDateTime &date)
{
	auto eph = getEphemeris(toJulianDay(date));
	double M = eph.l - eph.w;
	// Modulus the mean anomaly so that -180 < M < 180
	double m = (M + M_PI) / (2.0 * M_PI);
	M = (m - std::floor(m)) * 2.0 * M_PI - M_PI;
	return getEllipsePos(eph, M);
}

double PlanetImpl::getRotationAngle(const QDateTime &time)
{
	return -time.msecsTo(QDateTime(QDate(2000, 1, 1), QTime(0, 0))) / (1000.0 * 60.0 * 60.0 * 24.0 / 360.0) / _rotation_period;
}
