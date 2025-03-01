#pragma once

#include <vector>

#include "mmath.h"

struct Triangle {
    Vector3f vertices[3];   // v  顶点坐标
    Vector3f texCoords[3];  // vt 纹理坐标
    Vector3f normals[3];    // vn 法线
};