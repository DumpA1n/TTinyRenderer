#pragma once

#include "texture.h"

struct vertex_shader_payload {
    Vector3f position;
};

struct fragment_shader_payload {
    Vector3f view_pos;
    Vector3f color;
    Vector3f normal;
    Vector3f tex_coords;
    Texture* texture;
    fragment_shader_payload(const Vector3f& col, const Vector3f& nor, const Vector3f& tc, Texture* tex) :
         color(col), normal(nor), tex_coords(tc), texture(tex) {}
};

struct Light
{
    Vector3f position;
    Vector3f intensity;
};