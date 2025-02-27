#pragma once

#include <cmath>
#include <cstring>

template<int n, typename T> struct Vector {
    T data[n] = {0};
    T& operator[](int idx)       { assert(idx >= 0 && idx < n); return data[idx]; }
    T  operator[](int idx) const { assert(idx >= 0 && idx < n); return data[idx]; }

    Vector<n, T> operator+(T value) {
        Vector<n, T> vec;
        for (int i = 0; i < n; i++) { data[i] += value; }
        return vec;
    }
    Vector<n, T> operator-(T value) {
        Vector<n, T> vec;
        for (int i = 0; i < n; i++) { data[i] -= value; }
        return vec;
    }
    Vector<n, T> operator*(T value) {
        Vector<n, T> vec;
        for (int i = 0; i < n; i++) { data[i] *= value; }
        return vec;
    }
    Vector<n, T> operator/(T value) {
        for (int i = 0; i < n; i++) { assert(value != 0); data[i] /= value; }
        return this;
    }
    T norm() {
        T sum = 0;
        for (int i = 0; i < n; i++) { sum += data[i] * data[i]; }
        return (T)sqrt(sum);
    }
    Vector<n, T> normalized() { return *this / norm(); }
};

struct Vector3f : Vector<3, float> {
    float x = data[0], y = data[1], z = data[2];
    Vector3f cross(Vector3f& o) { return {y*o.z - z*o.y, z*o.x - x*o.z, x*o.y - y*o.x}; }
};

struct Vector3c : Vector<3, char> {
    char x = data[0], y = data[1], z = data[2];
};

struct Point : Vector3f {
    float x = data[0], y = data[1], z = 1.0f;
};

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
