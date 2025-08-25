#include "Mat4.hh"
#include <cmath>
    
Mat4::Mat4() {
    identity();
}

void Mat4::identity() {
    for (int i = 0; i < 16; i++) m[i] = 0;
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

Mat4 Mat4::operator*(const Mat4& other) const {
    Mat4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i*4 + j] = 0;
            for (int k = 0; k < 4; k++) {
                result.m[i*4 + j] += m[i*4 + k] * other.m[k*4 + j];
            }
        }
    }
    return result;
}

Mat4 Mat4::perspective(float fov, float aspect, float near, float far) {
    Mat4 result;
    float tanHalfFov = tan(fov / 2.0f);
    
    result.m[0] = 1.0f / (aspect * tanHalfFov);
    result.m[5] = 1.0f / tanHalfFov;
    result.m[10] = -(far + near) / (far - near);
    result.m[11] = -1.0f;
    result.m[14] = -(2.0f * far * near) / (far - near);
    result.m[15] = 0.0f;
    
    return result;
}

Mat4 Mat4::lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
    Vec3 f = (center - eye).normalize();
    Vec3 s = f.cross(up).normalize();
    Vec3 u = s.cross(f);
    
    Mat4 result;
    result.m[0] = s.x;
    result.m[4] = s.y;
    result.m[8] = s.z;
    result.m[1] = u.x;
    result.m[5] = u.y;
    result.m[9] = u.z;
    result.m[2] = -f.x;
    result.m[6] = -f.y;
    result.m[10] = -f.z;
    result.m[12] = -s.dot(eye);
    result.m[13] = -u.dot(eye);
    result.m[14] = f.dot(eye);
    
    return result;
}

Mat4 Mat4::translate(const Vec3& translation) {
    Mat4 result;
    result.m[12] = translation.x;
    result.m[13] = translation.y;
    result.m[14] = translation.z;
    return result;
}

Mat4 Mat4::ortho(float left, float right, float bottom, float top, float near, float far) {
    Mat4 result;
    
    result.m[0] = 2.0f / (right - left);
    result.m[5] = 2.0f / (top - bottom);
    result.m[10] = -2.0f / (far - near);
    result.m[12] = -(right + left) / (right - left);
    result.m[13] = -(top + bottom) / (top - bottom);
    result.m[14] = -(far + near) / (far - near);
    result.m[15] = 1.0f;
    
    return result;
}
