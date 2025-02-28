#pragma once

#include <cmath>
#include <cstring>

template<int n, typename T> struct Vector {
    T data[n] = {0};
    T& operator[](int idx)       { assert(idx >= 0 && idx < n); return data[idx]; }
    T  operator[](int idx) const { assert(idx >= 0 && idx < n); return data[idx]; }

    Vector<n, T> operator+(T value) const {
        Vector<n, T> result = *this;
        for (int i = 0; i < n; i++) { result.data[i] += value; }
        return result;
    }
    Vector<n, T> operator-(T value) const {
        Vector<n, T> result = *this;
        for (int i = 0; i < n; i++) { result.data[i] -= value; }
        return result;
    }
    Vector<n, T> operator*(T value) const {
        Vector<n, T> result = *this;
        for (int i = 0; i < n; i++) { result.data[i] *= value; }
        return result;
    }
    Vector<n, T> operator/(T value) const {
        if (value == 0) throw std::runtime_error("Division by zero");
        Vector<n, T> result;
        for (int i = 0; i < n; i++) { result.data[i] = data[i] / value; }
        return result;
    }
    Vector<n, T> operator+(const Vector<n, T>& other) const {
        Vector<n, T> result;
        for (int i = 0; i < n; i++) { result.data[i] = data[i] + other.data[i]; }
        return result;
    }
    Vector<n, T> operator-(const Vector<n, T>& other) const {
        Vector<n, T> result;
        for (int i = 0; i < n; i++) { result.data[i] = data[i] - other.data[i]; }
        return result;
    }
    inline double norm() {
        double sum = 0;
        for (int i = 0; i < n; i++) { sum += data[i] * data[i]; }
        return sqrt(sum);
    }
    inline Vector<n, T> normalized() { return *this / norm(); }
};

template<> struct Vector<3, float> {
    float x = 0, y = 0, z = 0;
};

template<> struct Vector<3, char> {
    char x = 0, y = 0, z = 0;
};

template<> struct Vector<2, int> {
    int x = 0, y = 0;
};

typedef Vector<3, float> Vector3f;
typedef Vector<3, char> Vector3c;
typedef Vector<2, int> Vector2i;

// struct Vector3f : Vector<3, float> {
//     float &x, &y, &z;
//     Vector3f() : x(data[0]), y(data[1]), z(data[2]) {}
//     Vector3f(float _x, float _y, float _z = 0.0f) : x(data[0]), y(data[1]), z(data[2]) {
//         x = _x;
//         y = _y;
//         z = _z;
//     }
//     Vector3f(const Vector3f& v) : x(data[0]), y(data[1]), z(data[2]) {
//         data[0] = v.x;
//         data[1] = v.y;
//         data[2] = v.z;
//     }
//     Vector3f& operator=(const Vector3f& v) {
//         if (this != &v) {
//             data[0] = v.x;
//             data[1] = v.y;
//             data[2] = v.z;
//         }
//         return *this;
//     }
//     Vector3f operator-(const Vector3f& other) const { return Vector3f(x - other.x, y - other.y, z - other.z); }
//     Vector3f cross(const Vector3f& o) const { return {y*o.z - z*o.y, z*o.x - x*o.z, x*o.y - y*o.x}; }
// };

// struct Vector3c : Vector<3, char> {
//     char &x, &y, &z;
//     Vector3c() : x(data[0]), y(data[1]), z(data[2]) {}
//     Vector3c(char _x, char _y, char _z = 0) : x(data[0]), y(data[1]), z(data[2]) {
//         x = _x;
//         y = _y;
//         z = _z;
//     }
//     Vector3c(const Vector3c& v) : x(data[0]), y(data[1]), z(data[2]) {
//         data[0] = v.x;
//         data[1] = v.y;
//         data[2] = v.z;
//     }
//     Vector3c& operator=(const Vector3c& v) {
//         if (this != &v) {
//             data[0] = v.x;
//             data[1] = v.y;
//             data[2] = v.z;
//         }
//         return *this;
//     }
// };

// struct Vector2i : Vector<2, int> {
//     int &x, &y;
//     Vector2i() : x(data[0]), y(data[1]) {}
//     Vector2i(int _x, int _y) : x(data[0]), y(data[1]) {
//         x = _x;
//         y = _y;
//     }
// };

struct Matrix4f {
    float m[4][4];

    Matrix4f() { memset(m, 0, sizeof(m)); }
    Matrix4f(const float* other) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                m[i][j] = other[i * 4 + j];
            }
        }
    }
    static Matrix4f identity() {
        Matrix4f im;
        for (int i = 0; i < 4; i++) {
            im.m[i][i] = 1.0f;
        }
        return im;
    }
    Matrix4f operator*(const Matrix4f& other) {
        Matrix4f nm;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                nm.m[i][j] = m[i][j] * other.m[i][j];
            }
        }
        return nm;
    }
};
