 #include "Vec3.hh"
#include <cmath>
 
Vec3 Vec3::operator+(const Vec3& other) const {
    return Vec3(x + other.x, y + other.y, z + other.z);
}

Vec3 Vec3::operator-(const Vec3& other) const {
    return Vec3(x - other.x, y - other.y, z - other.z);
}

Vec3 Vec3::operator*(float scalar) const {
    return Vec3(x * scalar, y * scalar, z * scalar);
}

float Vec3::length() const {
    return sqrt(x*x + y*y + z*z);
}

Vec3 Vec3::normalize() const {
    float len = length();
    if (len > 0) return Vec3(x/len, y/len, z/len);
    return Vec3(0, 0, 0);
}

float Vec3::dot(const Vec3& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vec3 Vec3::cross(const Vec3& other) const {
    return Vec3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}