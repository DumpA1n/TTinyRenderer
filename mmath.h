#pragma once

#include <cstdint>
#include <cmath>
#include <cstring>
#include <array>
#include <stdexcept>
#include <cassert>
#include <limits>

template<int n, typename T>
struct vec {
    T data[n];
    T  operator[](int i) const { assert(i >= 0 && i < n); return data[i]; }
    T& operator[](int i)       { assert(i >= 0 && i < n); return data[i]; }
};
template<int n, typename T>
vec<n, T> operator+(const vec<n, T>& lv, const vec<n, T>& rv) {
    auto ret = lv;
    for (int i = 0; i < n; i++) { ret[i] += rv[i]; }
    return ret;
}
template<int n, typename T>
vec<n, T>& operator+=(vec<n, T>& lv, const vec<n, T>& rv) {
    for (int i = 0; i < n; i++) { lv[i] += rv[i]; }
    return lv;
}
template<int n, typename T>
vec<n, T> operator-(const vec<n, T>& lv, const vec<n, T>& rv) {
    auto ret = lv;
    for (int i = 0; i < n; i++) { ret[i] -= rv[i]; }
    return ret;
}
template<int n, typename T>
vec<n, T>& operator-=(vec<n, T>& lv, const vec<n, T>& rv) {
    for (int i = 0; i < n; i++) { lv[i] -= rv[i]; }
    return lv;
}
template<int n, typename T>
T operator*(const vec<n, T>& lv, const vec<n, T>& rv) {
    T ret{0};
    for (int i = 0; i < n; i++) { ret += lv[i] * rv[i]; }
    return ret;
}
template<int n, typename T>
vec<n, T> operator*(const vec<n, T>& lv, const T& va) {
    auto ret = lv;
    for (int i = 0; i < n; i++) { ret[i] *= va; }
    return ret;
}
template<int n, typename T>
vec<n, T> operator*(const T& va, const vec<n, T>& lv) {
    return lv * va;
}
template<int n, typename T>
vec<n, T> operator/(const vec<n, T>& lv, const T& va) {
    auto ret = lv;
    for (int i = 0; i < n; i++) { ret[i] /= va; }
    return ret;
}

template<int n, typename T>
inline T norm(const vec<n, T>& v) {
    return std::sqrt(v*v);
}
template<int n, typename T>
inline T squaredNorm(const vec<n, T>& v) {
    return v*v;
}
template<int n, typename T>
inline vec<n, T> normalized(const vec<n, T>& v) {
    T nm = norm(v);
    return (nm > std::numeric_limits<T>::epsilon()) ? (v / nm) : v;
}
template<int n, typename T>
inline vec<n, T> cwiseProduct(const vec<n, T>& lv, const vec<n, T>& rv) {
    auto ret = lv;
    for (int i = 0; i < n; i++) { ret[i] *= rv[i]; }
    return ret;
}

template<typename T>
struct vec<2, T> {
    T x = 0, y = 0;
    vec() = default;
    vec(T va) : x(va), y(va) {}
    vec(T x, T y) : x(x), y(y) {}
    T  operator[](int i) const { assert(i >= 0 && i < 2); return reinterpret_cast<const T*>(this)[i]; }
    T& operator[](int i)       { assert(i >= 0 && i < 2); return reinterpret_cast<      T*>(this)[i]; }
};

template<typename T>
struct vec<3, T> {
    T x = 0, y = 0, z = 0;
    vec() = default;
    vec(T va) : x(va), y(va), z(va) {}
    vec(T x, T y, T z) : x(x), y(y), z(z) {}
    vec(const vec<2, T>& v2, T z) : x(v2.x), y(v2.y), z(z) {}
    T  operator[](int i) const { assert(i >= 0 && i < 3); return reinterpret_cast<const T*>(this)[i]; }
    T& operator[](int i)       { assert(i >= 0 && i < 3); return reinterpret_cast<      T*>(this)[i]; }
    vec<2, T> xy() { return {x, y}; }
};

template<typename T>
struct vec<4, T> {
    T x = 0, y = 0, z = 0, w = 0;
    vec() = default;
    vec(T va) : x(va), y(va), z(va), w(va) {}
    vec(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
    vec(const vec<3, T>& v3, T w) : x(v3.x), y(v3.y), z(v3.z), w(w) {}
    T  operator[](int i) const { assert(i >= 0 && i < 4); return reinterpret_cast<const T*>(this)[i]; }
    T& operator[](int i)       { assert(i >= 0 && i < 4); return reinterpret_cast<      T*>(this)[i]; }
    vec<2, T> xy() { return {x, y}; }
    vec<3, T> xyz() { return {x, y, z}; }
};

using vec2i = vec<2, int>;
using vec2f = vec<2, float>;
using vec3f = vec<3, float>;
using vec4f = vec<4, float>;
using vec3c = vec<3, uint8_t>;

// inline vec3f cross(const vec3f& lv, const vec3f& rv) { return {lv.y*rv.z - lv.z*rv.y, lv.z*rv.x - lv.x*rv.z, lv.x*rv.y - lv.y*rv.x}; }

using Vector2i = vec<2, int>;
using Vector2f = vec<2, float>;
using Vector3f = vec<3, float>;
using Vector4f = vec<4, float>;
using Vector3c = vec<3, uint8_t>;

inline Vector3f cross(const Vector3f& lv, const Vector3f& rv) { return {lv.y*rv.z - lv.z*rv.y, lv.z*rv.x - lv.x*rv.z, lv.x*rv.y - lv.y*rv.x}; }

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
        return Vector3f{x_new, y_new, z_new};
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
        return Vector3f{x_new, y_new, z_new};
    }
    Vector4f operator*(const Vector4f& v) const {
        float x_new = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
        float y_new = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
        float z_new = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
        float w_new = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;
        return Vector4f{x_new, y_new, z_new, w_new};
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
