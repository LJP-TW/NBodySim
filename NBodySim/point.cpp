#include <random>

#include <stdlib.h>
#include <time.h>

#include "option.h"
#include "point.h"

#ifdef _DEBUG
#	include <iostream>
using namespace std;
#endif

point::point()
{
	unsigned int seed = time(NULL);
	std::default_random_engine generator(seed);
	std::uniform_real_distribution<float> unif(-1.0, 1.0);
	float mass_x;

	_x = unif(generator) * POINT_XYZ_MAX;
	_y = unif(generator) * POINT_XYZ_MAX;
	_z = unif(generator) * POINT_XYZ_MAX;
	mass_x = (unif(generator) + 1.001) / 2.0;
	_mass = mass_x * POINT_MASS_MAX;
	_size = mass_x * POINT_SIZE_MAX;
	_r = 0.3 * mass_x + 0.8;
	_g = 1.348 * mass_x * mass_x - 0.385 * mass_x + 0.342;
	_b = 0.34 * mass_x * mass_x + 0.883 * mass_x - 0.07;
	_sx = unif(generator) * (1.001 - mass_x) * POINT_SPEED_MAX;
	_sy = unif(generator) * (1.001 - mass_x) * POINT_SPEED_MAX;
	_sz = unif(generator) * (1.001 - mass_x) * POINT_SPEED_MAX;

#ifdef _DEBUG
	log(seed);
#endif
}

point::point(unsigned int seed)
{
	std::default_random_engine generator(seed);
	std::uniform_real_distribution<float> unif(-1.0, 1.0);
	float mass_x;

	_x = unif(generator) * POINT_XYZ_MAX;
	_y = unif(generator) * POINT_XYZ_MAX;
	_z = unif(generator) * POINT_XYZ_MAX;
	mass_x = (unif(generator) + 1.001) / 2.0;
	_mass = mass_x * POINT_MASS_MAX;
	_size = mass_x * POINT_SIZE_MAX;
	_r = 0.3 * mass_x + 0.8;
	_g = 1.348 * mass_x * mass_x - 0.385 * mass_x + 0.342;
	_b = 0.34 * mass_x * mass_x + 0.883 * mass_x - 0.07;
	_sx = unif(generator) * (1.001 - mass_x) * POINT_SPEED_MAX;
	_sy = unif(generator) * (1.001 - mass_x) * POINT_SPEED_MAX;
	_sz = unif(generator) * (1.001 - mass_x) * POINT_SPEED_MAX;

#ifdef _DEBUG
	log(seed);
#endif
}

point::point(float x, float y, float z, float r, float g, float b, float size, 
	float mass, double sx, double sy, double sz) :
	_x(x), _y(y), _z(z), _r(r), _g(g), _b(b), _size(size), _mass(mass),
	_sx(sx), _sy(sy), _sz(sz)
{
#ifdef _DEBUG
	cout << "seed (Nan) ";
	log();
#endif
}

#ifdef _DEBUG
void point::log()
{
	cout << "x, y, z, r, g, b, size, sx, sy, sz, mass = "
		<< _x << ", "
		<< _y << ", "
		<< _z << ", "
		<< _r << ", "
		<< _g << ", "
		<< _b << ", "
		<< _size << ", "
		<< _sx << ", "
		<< _sy << ", "
		<< _sz << ", "
		<< _mass << endl;
}

void point::log(unsigned int seed)
{
	cout << "seed (" << seed << ") ";
	log();
}
#endif