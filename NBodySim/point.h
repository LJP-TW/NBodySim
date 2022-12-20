#pragma once

struct point
{
    point();
    point(unsigned int seed);
    point(float x, float y, float z, float r, float g, float b, float size,
        float mass, float sx, float sy, float sz);

#ifdef _DEBUG
    void log();
    void log(unsigned int seed);
#endif

    float _x, _y, _z, _padding1;
    float _r, _g, _b;
    float _size;
    float _sx, _sy, _sz, _padding2; // speed
    float _mass;
};