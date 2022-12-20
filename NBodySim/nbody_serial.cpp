#include <math.h>

#include "option.h"
#include "nbody.h"

// Ref: https://yangwc.com/2019/06/20/NbodySimulation/
// TODO: Add z-axis
static void _nBodyCalculate(const point *currpoints, point *newpoint, int i, double dt)
{
	double G = GRAVITATIONAL_G;
	double epi = 0.0000000000001;
	// Acceleration
	double ax = 0;
	double ay = 0;
	// double az = 0;

	for (int j = 0; j < POINT_CNT; ++j) {
		double rx;
		double ry;
		// double rz;
		double den; // denominator

		rx = currpoints[j]._x - currpoints[i]._x;
		ry = currpoints[j]._y - currpoints[i]._y;
		// rz = currpoints[j]._z - currpoints[i]._z;

		den = sqrt(pow(rx * rx + ry * ry + epi, 3.0));
		// den = sqrt(pow(rx * rx + ry * ry + rz * rz + epi, 3.0));

		ax += currpoints[j]._mass * rx / den;
		ay += currpoints[j]._mass * ry / den;
		// az += currpoints[j]._mass * rz / den;
	}

	ax *= G;
	ay *= G;
	// az *= G;

	// Update speed
	newpoint->_sx = currpoints[i]._sx + ax * dt;
	newpoint->_sy = currpoints[i]._sy + ay * dt;
	// newpoint->_sz = currpoints[i]._sz + az * dt;

	// Update position
	newpoint->_x = currpoints[i]._x + newpoint->_sx * dt;
	newpoint->_y = currpoints[i]._y + newpoint->_sy * dt;
	// newpoint->_z = currpoints[i]._z + newpoint->_sz * dt;
}

void nBodyCalculateSerial(const point *currpoints, point *newpoints, double dt)
{
	for (int i = 0; i < POINT_CNT; ++i) {
		_nBodyCalculate(currpoints, &newpoints[i], i, dt);

#ifdef VERBOSE
		newpoints[i].log();
#endif
	}

#ifdef VERBOSE
	printf("\n");
#endif
}
