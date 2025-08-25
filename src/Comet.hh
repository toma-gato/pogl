#pragma once

#include "Vec3.hh"

class Comet {
public:
    Vec3 position;
    Vec3 velocity;
    float size;
    Vec3 color;
    
    Comet(Vec3 position = Vec3(-15, 0, 0), Vec3 velocity = Vec3(2, 0.5f, 0), float size = 0.1f, Vec3 color = Vec3(0.2f, 0.4f, 0.9f))
        : position(position), velocity(velocity), size(size), color(color) {}  
};