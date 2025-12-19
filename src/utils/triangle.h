#pragma once

#include <array>

#include "mmath.h"

struct Triangle {
    std::array<Vector4f, 3> vertices;   // v  顶点坐标
    std::array<Vector2f, 3> tex_coords; // vt 纹理坐标
    std::array<Vector3f, 3> normals;    // vn 法线
    std::array<Vector3f, 3> color;      // base color
    Triangle() {
        vertices[0] = Vector4f{0.0f, 0.0f, 0.0f, 1.0f};
        vertices[1] = Vector4f{0.0f, 0.0f, 0.0f, 1.0f};
        vertices[2] = Vector4f{0.0f, 0.0f, 0.0f, 1.0f};
        color[0] = Vector3f{1.0f};
        color[1] = Vector3f{1.0f};
        color[2] = Vector3f{1.0f};
    }
    std::array<Vector2f, 3> toVector2f() const {
        return { vertices[0].xy(), vertices[1].xy(), vertices[2].xy() };
    }
    std::array<Vector3f, 3> toVector3f() const {
        return { vertices[0].xyz(), vertices[1].xyz(), vertices[2].xyz() };
    }
    std::array<Vector4f, 3> toVector4f() const {
        return { vertices[0], vertices[1], vertices[2] };
    }
    void setVertices(int index, const Vector3f& v) {
        vertices[index] = Vector4f{v, 1.0f};
    }
    void setTexCoords(int index, const Vector2f& v) {
        tex_coords[index] = v;
    }
    void setNormals(int index, const Vector3f& v) {
        normals[index] = v;
    }
    void setColor(int index, const Vector3f& v) {
        color[index] = v;
    }
};
