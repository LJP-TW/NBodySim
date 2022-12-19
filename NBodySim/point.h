#pragma once

struct point
{
    point();
    point(unsigned int seed);
    point(float x, float y, float z, float r, float g, float b, float size);

    float _x, _y, _z;
    float _r, _g, _b;
    float _size;
};