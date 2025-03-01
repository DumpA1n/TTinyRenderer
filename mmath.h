#pragma once

#include <cmath>
#include <cstring>
#include <array>

struct Vector2i {
    int x, y;
    Vector2i() : x(0), y(0) {}
    Vector2i(int v) : x(v), y(v) {}
    Vector2i(int _x, int _y) : x(_x), y(_y) {}
};

struct Vector3f {
    float x, y, z;
    Vector3f() : x(0), y(0), z(0) {}
    Vector3f(float v) : x(v), y(v), z(v) {}
    Vector3f(int _x, int _y) : x(static_cast<float>(_x)), y(static_cast<float>(_y)), z(0) {}
    Vector3f(float _x, float _y, float _z = 0.0f) : x(_x), y(_y), z(_z) {}
    Vector3f operator+(const Vector3f& o) const { return Vector3f{x + o.x, y + o.y, z + o.z}; }
    Vector3f operator-(const Vector3f& o) const { return Vector3f{x - o.x, y - o.y, z - o.z}; }
    Vector3f operator*(const Vector3f& o) const { return Vector3f{x * o.x, y * o.y, z * o.z}; }
    Vector3f operator/(float value) const { return {x / value, y / value, z / value}; }
    Vector3f& operator+=(const Vector3f& o) { x += o.x; y += o.y; z += o.z; return *this; }
    inline float norm() const { return std::sqrt(x*x + y*y + z*z); }
    inline float squaredNorm() const { return x*x + y*y + z*z; }
    inline Vector3f normalized() const { return *this / norm(); }
    inline Vector3f cwiseProduct(const Vector3f& o) const { return {x*o.x, y*o.y, z*o.z}; }
    inline float dot(const Vector3f& o) const { return x * o.x + y * o.y + z * o.z; }
    inline Vector3f cross(const Vector3f& o) const { return {y*o.z - z*o.y, z*o.x - x*o.z, x*o.y - y*o.x}; }
};

struct Vector4f {
    float x, y, z, w;
    Vector4f() : x(0), y(0), z(0), w(0) {}
    Vector4f(float v) : x(v), y(v), z(v), w(v) {}
    Vector4f(float _x, float _y, float _z, float _w = 0.0f) : x(_x), y(_y), z(_z), w(_w) {}
    Vector4f(int _x, int _y, int _z) : x(static_cast<float>(_x)), y(static_cast<float>(_y)), z(static_cast<float>(_z)), w(0) {}
    Vector4f(const Vector3f o, float _w) : x(o.x), y(o.y), z(o.z), w(_w) {}
};

struct Vector3c {
    char x, y, z;
    Vector3c() : x(0), y(0), z(0) {}
    Vector3c(char v) : x(v), y(v), z(v) {}
    Vector3c(char _x, char _y, char _z = 0) : x(_x), y(_y), z(_z) {}
};

struct Matrix4f {
    std::array<std::array<float, 4>, 4> m;
    Matrix4f() { memset(m.data(), 0, sizeof(m)); }
    Matrix4f(float m00, float m01, float m02, float m03,
             float m10, float m11, float m12, float m13,
             float m20, float m21, float m22, float m23,
             float m30, float m31, float m32, float m33) {
        m = {{{m00, m01, m02, m03}, 
              {m10, m11, m12, m13}, 
              {m20, m21, m22, m23}, 
              {m30, m31, m32, m33}}};
    }
    Matrix4f(const Vector4f& row0, const Vector4f& row1, const Vector4f& row2, const Vector4f& row3) {
        m = {{{row0.x, row0.y, row0.z, row0.w},
              {row1.x, row1.y, row1.z, row1.w},
              {row2.x, row2.y, row2.z, row2.w},
              {row3.x, row3.y, row3.z, row3.w}}};
    }
    static Matrix4f identity() {
        Matrix4f im;
        for (int i = 0; i < 4; i++) {
            im.m[i][i] = 1.0f;
        }
        return im;
    }
    Matrix4f operator*(const Matrix4f& other) {
        Matrix4f result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[i][j] = 0;
                for (int k = 0; k < 4; k++) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }
    Vector3f operator*(const Vector3f& v) const {
        float x_new = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * 1.0f;
        float y_new = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * 1.0f;
        float z_new = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * 1.0f;
        return Vector3f(x_new, y_new, z_new);
    }
};
