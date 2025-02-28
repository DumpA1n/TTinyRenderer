#pragma once

#include "mmath.h"

Matrix4f rotateX(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return Matrix4f(
        Vector4f{1,  0,  0, 0},
        Vector4f{0,  c, -s, 0},
        Vector4f{0,  s,  c, 0},
        Vector4f{0,  0,  0, 1}
    );
}

Matrix4f rotateY(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return Matrix4f{
        Vector4f{ c,  0,  s, 0},
        Vector4f{ 0,  1,  0, 0},
        Vector4f{-s,  0,  c, 0},
        Vector4f{ 0,  0,  0, 1}
    };
}

Matrix4f rotateZ(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return Matrix4f{
        Vector4f{ c, -s,  0, 0},
        Vector4f{ s,  c,  0, 0},
        Vector4f{ 0,  0,  1, 0},
        Vector4f{ 0,  0,  0, 1}
    };
}

Matrix4f get_transform_matrix(Vector3f scale, Vector3f rotate, Vector3f model) {
    Matrix4f mscale(
        Vector4f{scale.x, 0, 0, 0},
        Vector4f{0, scale.y, 0, 0},
        Vector4f{0, 0, scale.z, 0},
        Vector4f{0, 0, 0, 1}
    );
    Matrix4f mrotate = rotateX(rotate.x) * rotateY(rotate.y) * rotateZ(rotate.z);
    Matrix4f mmodel{
        Vector4f{1, 0, 0, model.x},
        Vector4f{0, 1, 0, model.y},
        Vector4f{0, 0, 1, model.z},
        Vector4f{0, 0, 0, 1}
    };
    return mmodel * mrotate * mscale;
}

struct vertex_shader_payload {
    Vector3f position;
};

Vector3f default_vertex_shader(vertex_shader_payload payload) {
    return get_transform_matrix(Vector3f{1.0f}, {10.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 5.0f}) * payload.position;
}