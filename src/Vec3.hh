#pragma once

class Vec3 {
public:
    float x;
    float y;
    float z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    
    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;
    Vec3 operator*(float scalar) const;
    
    float length() const;
    Vec3 normalize() const;
    float dot(const Vec3& other) const;
    Vec3 cross(const Vec3& other) const;
};