#include <omp.h>

#include <math.h>
#include <intrin.h>

#include "option.h"
#include "nbody.h"

#ifdef VERBOSE
#include <iostream>
using namespace std;
static int is_msg_show;
#endif

static void _nBodyCalculate(const point *currpoints, point *newpoint, int i, double dt)
{
	double G = GRAVITATIONAL_G;
	__m128 vepi;
	__m128 vpow;
	// Acceleration
	__m128 vaxyz;
	__m128 vG;
	__m128 vdt;
	__m128 vsxyz;
	__m128 vpxyz;
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

	vaxyz = _mm_mul_ps(vaxyz, vG);

	// Update speed
	vsxyz = _mm_load_ps(&currpoints[i]._sx);
	vtmp = _mm_mul_ps(vaxyz, vdt);
	vsxyz = _mm_add_ps(vsxyz, vtmp);
	_mm_store_ps(&newpoint->_sx, vsxyz);

	// Update position
	vpxyz = _mm_load_ps(&currpoints[i]._x);
	vtmp = _mm_mul_ps(vsxyz, vdt);
	vpxyz = _mm_add_ps(vpxyz, vtmp);
	_mm_store_ps(&newpoint->_x, vpxyz);
}

void nBodyCalculateParallelSIMD(const point *currpoints, point *newpoints, double dt)
{
	#pragma omp parallel for
	for (int i = 0; i < POINT_CNT; ++i) {
		_nBodyCalculate(currpoints, &newpoints[i], i, dt);
	}

#ifdef _DEBUG
#	ifdef VERBOSE
	if (!is_msg_show) {
		cout << "OMP Max threads: " << omp_get_max_threads() << endl;
		is_msg_show = 1;
	}
#	endif
#endif
}