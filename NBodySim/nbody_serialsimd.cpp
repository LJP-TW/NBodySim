#include <math.h>
#include <intrin.h>

#include "option.h"
#include "nbody.h"

// TODO: Add z-axis
static void _nBodyCalculate(const point *currpoints, point *newpoint, int i, double dt)
{
	double G = GRAVITATIONAL_G;
	__m128 vepi;
	__m128 vpow;
	// Acceleration
	double ax = 0;
	double ay = 0;
	// double az = 0;
	__m128 vaxyz;
	__m128 vG;
	__m128 vdt;
	__m128 vsxy;
	__m128 vtmp;

	vaxyz = _mm_set_ps1(0.0);
	vepi = _mm_set_ps1(0.0000000000001);
	vpow = _mm_set_ps1(-1.5);
	vG = _mm_set_ps1(G);
	vdt = _mm_set_ps1(dt);

	for (int j = 0; j < POINT_CNT; ++j) {
		__m128 vrxyz, vjxyz, vixyz;
		__m128 vden;
		__m128 vmass;

		vjxyz = _mm_load_ps(&currpoints[j]._x);
		vixyz = _mm_load_ps(&currpoints[i]._x);
		vrxyz = _mm_sub_ps(vjxyz, vixyz);

		vden = _mm_mul_ps(vrxyz, vrxyz);

		// (a1, a2, a3, a4) hadd (a1, a2, a3, a4) = (a1+a2, a3+a4, a1+a2, a3+a4)
		vden = _mm_hadd_ps(vden, vden);
		vden = _mm_hadd_ps(vden, vden);
		vden = _mm_add_ps(vden, vepi);
		vden = _mm_pow_ps(vden, vpow);

		vmass = _mm_set_ps1(currpoints[j]._mass);
		vmass = _mm_mul_ps(vmass, vrxyz);
		vmass = _mm_mul_ps(vmass, vden);
		vaxyz = _mm_add_ps(vaxyz, vmass);
	}

	// ax *= G;
	// ay *= G;
	// az *= G;
	vaxyz = _mm_mul_ps(vaxyz, vG);

	// Update speed
	// newpoint->_sx = currpoints[i]._sx + ax * dt;
	// newpoint->_sy = currpoints[i]._sy + ay * dt;
	// newpoint->_sz = currpoints[i]._sz + az * dt;
	vsxy = _mm_load_ps(&currpoints[i]._sx);
	vtmp = _mm_mul_ps(vaxyz, vdt);

	vsxy = _mm_add_ps(vsxy, vtmp);


	// Update position
	newpoint->_x = currpoints[i]._x + newpoint->_sx * dt;
	newpoint->_y = currpoints[i]._y + newpoint->_sy * dt;
	// newpoint->_z = currpoints[i]._z + newpoint->_sz * dt;
}

void nBodyCalculateSerialSIMD(const point *currpoints, point *newpoints, double dt)
{
	for (int i = 0; i < POINT_CNT; ++i) {
		_nBodyCalculate(currpoints, &newpoints[i], i, dt);
	}
}
