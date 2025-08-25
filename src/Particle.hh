#pragma once

#include "Vec3.hh"

class Vec3;

class Particle {
public:
    Vec3 position;
    Vec3 velocity;
    Vec3 color;
    float life;
    float maxLife;
    float size;
    
    Particle() : life(0), maxLife(1), size(1) {}
    
    void update(float deltaTime);
    bool isAlive() const;
};