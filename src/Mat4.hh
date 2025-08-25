#pragma once

#include "Vec3.hh"

class Vec3;

class Mat4 {
public:
    float m[16];
    
    Mat4();

    void identity();
    
    Mat4 operator*(const Mat4& other) const;
    
    static Mat4 perspective(float fov, float aspect, float near, float far);
    
    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up);
    
    static Mat4 translate(const Vec3& translation);

    static Mat4 ortho(float left, float right, float bottom, float top, float near, float far);
};