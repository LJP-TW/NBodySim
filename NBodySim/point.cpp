#include <random>

#include <stdlib.h>
#include <time.h>

#include "point.h"

#ifdef _DEBUG
#	include <iostream>
using namespace std;
#endif

#define POINT_SIZE_MAX 50.0

point::point()
{
	unsigned int seed = time(NULL);
	std::default_random_engine generator(seed);
	std::uniform_real_distribution<float> unif(-1.0, 1.0);

	_x = unif(generator);
	_y = unif(generator);
	_z = unif(generator);
	_r = unif(generator);
	_g = unif(generator);
	_b = unif(generator);
	_size = (unif(generator) + 1.001) * POINT_SIZE_MAX;

#ifdef _DEBUG
	cout << "seed, x, y, z, r, g, b, size = "
		 << seed << ", "
		 << _x << ", "
		 << _y << ", "
		 << _z << ", "
		 << _r << ", "
		 << _g << ", "
		 << _b << ", "
		 << _size << endl;
#endif
}

point::point(unsigned int seed)
{
	std::default_random_engine generator(seed);
	std::uniform_real_distribution<float> unif(-1.0, 1.0);

	_x = unif(generator);
	_y = unif(generator);
	_z = unif(generator);
	_r = unif(generator);
	_g = unif(generator);
	_b = unif(generator);
	_size = (unif(generator) + 1.001) * POINT_SIZE_MAX;

#ifdef _DEBUG
	cout << "seed, x, y, z, r, g, b, size = "
		<< seed << ", "
		<< _x << ", "
		<< _y << ", "
		<< _z << ", "
		<< _r << ", "
		<< _g << ", "
		<< _b << ", "
		<< _size << endl;
#endif
}

point::point(float x, float y, float z, float r, float g, float b, float size) : 
	_x(x), _y(y), _z(z), _r(r), _g(g), _b(b), _size(size)
{
#ifdef _DEBUG
	cout << "seed, x, y, z, r, g, b, size = "
		<< "Nan, "
		<< _x << ", "
		<< _y << ", "
		<< _z << ", "
		<< _r << ", "
		<< _g << ", "
		<< _b << ", "
		<< _size << endl;
#endif
}