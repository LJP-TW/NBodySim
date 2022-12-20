#pragma once
#include "point.h"

typedef void (*nBodyFunc)(const point *currpoints, point *newpoints, double dt);

void nBodyCalculateSerial(const point *currpoints, point *newpoints, double dt);

void nBodyCalculateSerialSIMD(const point *currpoints, point *newpoints, double dt);
