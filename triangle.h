#pragma once

#include <vector>

#include "mmath.h"
#include "texture.h"

struct Triangle {
    Vector3f vertices[3];   // v  顶点坐标
    Vector3f color[3];      // base color
    Vector3f texCoords[3];  // vt 纹理坐标
    Vector3f normals[3];    // vn 法线
    Vector4f Vec4Vertices[3];
    Triangle() {
        color[0] = Vector3f{1.0f};
        color[1] = Vector3f{1.0f};
        color[2] = Vector3f{1.0f};
    }
    Vector4f* toVector4f() {
        Vector4f v[3] = {
            {vertices[0], 1.0f},
            {vertices[1], 1.0f},
            {vertices[2], 1.0f}
        };
        return v;
    }
};