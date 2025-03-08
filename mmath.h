#pragma once

#include <cmath>
#include <cstring>
#include <array>
#include <stdexcept>

struct Vector2i {
    int x, y;
    Vector2i() : x(0), y(0) {}
    Vector2i(int v) : x(v), y(v) {}
    Vector2i(int _x, int _y) : x(_x), y(_y) {}
};

struct Vector2f {
    float x, y;
    Vector2f() : x(0), y(0) {}
    Vector2f(float v) : x(v), y(v) {}
    Vector2f(float _x, float _y) : x(_x), y(_y) {}
    Vector2f uv() const { return {x, y}; }
};

struct Vector3f {
    float x, y, z;
    Vector3f() : x(0), y(0), z(0) {}
    Vector3f(float v) : x(v), y(v), z(v) {}
    Vector3f(int _x, int _y) : x(static_cast<float>(_x)), y(static_cast<float>(_y)), z(0) {}
    Vector3f(float _x, float _y, float _z = 0.0f) : x(_x), y(_y), z(_z) {}
    Vector3f operator+(const Vector3f& o) const { return Vector3f{x + o.x, y + o.y, z + o.z}; }
    Vector3f operator-(const Vector3f& o) const { return Vector3f{x - o.x, y - o.y, z - o.z}; }
    Vector3f operator*(float value) const { return Vector3f{x * value, y * value, z * value}; }
    // Vector3f operator*(float value, const Vector3f& v) { return Vector3f{v.x * value, v.y * value, v.z * value}; }
    Vector3f operator/(float value) const { return {x / value, y / value, z / value}; }
    Vector3f& operator+=(const Vector3f& o) { x += o.x; y += o.y; z += o.z; return *this; }
    inline float norm() const { return std::sqrt(x*x + y*y + z*z); }
    inline float squaredNorm() const { return x*x + y*y + z*z; }
    inline Vector3f normalized() const { return *this / norm(); }
    inline Vector3f cwiseProduct(const Vector3f& o) const { return {x*o.x, y*o.y, z*o.z}; }
    inline float dot(const Vector3f& o) const { return x * o.x + y * o.y + z * o.z; }
    inline Vector3f cross(const Vector3f& o) const { return {y*o.z - z*o.y, z*o.x - x*o.z, x*o.y - y*o.x}; }
    Vector2f xy() const { return {x, y}; }
};

struct Vector4f {
    float x, y, z, w;
    Vector4f() : x(0), y(0), z(0), w(0) {}
    Vector4f(float v) : x(v), y(v), z(v), w(v) {}
    Vector4f(float _x, float _y, float _z, float _w = 0.0f) : x(_x), y(_y), z(_z), w(_w) {}
    Vector4f(int _x, int _y, int _z) : x(static_cast<float>(_x)), y(static_cast<float>(_y)), z(static_cast<float>(_z)), w(0) {}
    Vector4f(const Vector3f o, float _w) : x(o.x), y(o.y), z(o.z), w(_w) {}
    Vector2f xy() const { return {x, y}; }
    Vector3f xyz() const { return {x, y, z}; }
};

struct Vector3c {
    uint8_t x, y, z;
    Vector3c() : x(0), y(0), z(0) {}
    Vector3c(uint8_t v) : x(v), y(v), z(v) {}
    Vector3c(uint8_t _x, uint8_t _y, uint8_t _z = 0) : x(_x), y(_y), z(_z) {}
};

struct Matrix3f {
    std::array<std::array<float, 3>, 3> m;
    Matrix3f() { memset(m.data(), 0, sizeof(m)); }
    Matrix3f(float m00, float m01, float m02,
             float m10, float m11, float m12,
             float m20, float m21, float m22) {
        m = {{{m00, m01, m02}, 
              {m10, m11, m12}, 
              {m20, m21, m22}}};
    }
    Matrix3f(const Vector3f& row0, const Vector3f& row1, const Vector3f& row2) {
        m = {{{row0.x, row0.y, row0.z},
              {row1.x, row1.y, row1.z},
              {row2.x, row2.y, row2.z}}};
    }
    static Matrix3f identity() {
        Matrix3f im;
        memset(im.m.data(), 0, sizeof(im.m));
        for (int i = 0; i < 3; i++) {
            im.m[i][i] = 1.0f;
        }
        return im;
    }
    Matrix3f operator*(const Matrix3f& other) {
        Matrix3f result;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                result.m[i][j] = 0;
                for (int k = 0; k < 3; k++) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }
    Vector3f operator*(const Vector3f& v) const {
        float x_new = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z;
        float y_new = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z;
        float z_new = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z;
        return Vector3f(x_new, y_new, z_new);
    }
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
        memset(im.m.data(), 0, sizeof(im.m));
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
        float x_new = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z;
        float y_new = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z;
        float z_new = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z;
        return Vector3f(x_new, y_new, z_new);
    }
    Vector4f operator*(const Vector4f& v) const {
        float x_new = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
        float y_new = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
        float z_new = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
        float w_new = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;
        return Vector4f(x_new, y_new, z_new, w_new);
    }

    Matrix4f transpose() const {
        Matrix4f result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[i][j] = m[j][i];
            }
        }
        return result;
    }
    
    // Calculate determinant of 3x3 submatrix
    float det3(float a00, float a01, float a02,
        float a10, float a11, float a12,
        float a20, float a21, float a22) const {
    return a00 * (a11 * a22 - a12 * a21) -
        a01 * (a10 * a22 - a12 * a20) +
        a02 * (a10 * a21 - a11 * a20);
    }

    // Calculate inverse of Matrix4f
    Matrix4f inverse() const {
        // Calculate cofactors and determinant
        float c00 = det3(m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
        float c01 = -det3(m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
        float c02 = det3(m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
        float c03 = -det3(m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);

        float c10 = -det3(m[0][1], m[0][2], m[0][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
        float c11 = det3(m[0][0], m[0][2], m[0][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
        float c12 = -det3(m[0][0], m[0][1], m[0][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
        float c13 = det3(m[0][0], m[0][1], m[0][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);

        float c20 = det3(m[0][1], m[0][2], m[0][3], m[1][1], m[1][2], m[1][3], m[3][1], m[3][2], m[3][3]);
        float c21 = -det3(m[0][0], m[0][2], m[0][3], m[1][0], m[1][2], m[1][3], m[3][0], m[3][2], m[3][3]);
        float c22 = det3(m[0][0], m[0][1], m[0][3], m[1][0], m[1][1], m[1][3], m[3][0], m[3][1], m[3][3]);
        float c23 = -det3(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[3][0], m[3][1], m[3][2]);

        float c30 = -det3(m[0][1], m[0][2], m[0][3], m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3]);
        float c31 = det3(m[0][0], m[0][2], m[0][3], m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3]);
        float c32 = -det3(m[0][0], m[0][1], m[0][3], m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3]);
        float c33 = det3(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]);

        // Calculate determinant using first row cofactors
        float det = m[0][0] * c00 + m[0][1] * c01 + m[0][2] * c02 + m[0][3] * c03;

        // Check if the matrix is invertible
        if (std::abs(det) < 1e-6f) {
            // Return identity matrix or throw an exception for non-invertible matrix
            return identity();  // Or handle error appropriately
        }

        // Calculate inverse by dividing adjugate matrix by determinant
        float invDet = 1.0f / det;
        Matrix4f result{
            c00 * invDet, c10 * invDet, c20 * invDet, c30 * invDet,
            c01 * invDet, c11 * invDet, c21 * invDet, c31 * invDet,
            c02 * invDet, c12 * invDet, c22 * invDet, c32 * invDet,
            c03 * invDet, c13 * invDet, c23 * invDet, c33 * invDet
        };

        return result;
    }
};
