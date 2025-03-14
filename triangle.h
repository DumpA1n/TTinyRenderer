#pragma once

#include <vector>

#include "mmath.h"
#include "texture.h"

struct Triangle {
    Vector4f vertices[3];   // v  顶点坐标
    Vector2f tex_coords[3]; // vt 纹理坐标
    Vector3f normals[3];    // vn 法线
    Vector3f color[3];      // base color
    Triangle() {
        vertices[0] = Vector4f{0.0f, 0.0f, 0.0f, 1.0f};
        vertices[1] = Vector4f{0.0f, 0.0f, 0.0f, 1.0f};
        vertices[2] = Vector4f{0.0f, 0.0f, 0.0f, 1.0f};
        color[0] = Vector3f{1.0f};
        color[1] = Vector3f{1.0f};
        color[2] = Vector3f{1.0f};
    }
    std::array<Vector2f, 3> toVector2f() {
        return { vertices[0].xy(), vertices[1].xy(), vertices[2].xy() };
    }
    std::array<Vector3f, 3> toVector3f() {
        return { vertices[0].xyz(), vertices[1].xyz(), vertices[2].xyz() };
    }
    std::array<Vector4f, 3> toVector4f() {
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