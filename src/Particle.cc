#include "Particle.hh"

#include <cmath>

Vec3 position;
Vec3 velocity;
Vec3 color;
float life;
float maxLife;
float size;

void Particle::update(float deltaTime) {
    if (life > 0) {
        life -= deltaTime;
        position = position + velocity * deltaTime;
        
        float lifeFactor = std::pow(life / maxLife, 0.5f);
        color = color * (0.2f + 0.8f * lifeFactor);
        size = size * (0.3f + 0.7f * lifeFactor);

        velocity.y += (std::sin(life * 3.0f) * deltaTime * 0.1f);
        velocity.x += (std::cos(life * 3.0f) * deltaTime * 0.1f);
    }
}

bool Particle::isAlive() const {
    return life > 0;
}
