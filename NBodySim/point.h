#pragma once

struct point
{
    point();
    point(unsigned int seed);
    point(float x, float y, float z, float r, float g, float b, float size,
        float mass, double sx, double sy, double sz);

#ifdef _DEBUG
    void log();
    void log(unsigned int seed);
#endif

    float _x, _y, _z;
    float _r, _g, _b;
    float _size;
    float _mass;
    double _sx, _sy, _sz; // speed
};