#pragma once

#define DEMO_GRAVITATIONAL_SLINGSHOT
// #define DEMO_BINARY_STAR_SYSTEM

// #define VERBOSE

#ifdef DEMO_GRAVITATIONAL_SLINGSHOT
#   define SEED 12345678 - 100 * i
#   define DELTA_TIME_MUL 2000
#	define POINT_CNT 2
#	define POINT_XYZ_MAX  10000.0
#	define POINT_SIZE_MAX 50.0
#	define POINT_MASS_MAX 100.0
#	define POINT_SPEED_MAX 1.0
#endif

#ifdef DEMO_BINARY_STAR_SYSTEM
#   define SEED 5566888 - 100 * i
#   define DELTA_TIME_MUL 500
#	define POINT_CNT 15
#	define POINT_XYZ_MAX  10000.0
#	define POINT_SIZE_MAX 50.0
#	define POINT_MASS_MAX 100.0
#	define POINT_SPEED_MAX 1.0
#endif

#ifndef DELTA_TIME_MUL
#	define DELTA_TIME_MUL 1000
#endif

#ifndef POINT_CNT
#	define POINT_CNT 15
#endif

#ifndef POINT_XYZ_MAX
#	define POINT_XYZ_MAX  10000.0
#endif

#ifndef POINT_SIZE_MAX
#	define POINT_SIZE_MAX 50.0
#endif

#ifndef POINT_MASS_MAX
#	define POINT_MASS_MAX 100.0
#endif

#ifndef POINT_SPEED_MAX
#	define POINT_SPEED_MAX 1.0
#endif
