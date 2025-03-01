#pragma once

struct vertex_shader_payload {
    Vector3f position;
};

struct fragment_shader_payload {
    Vector3f view_pos;
    Vector3f color;
    Vector3f normal;
    Vector3f tex_coords;
    fragment_shader_payload(const Vector3f& col, const Vector3f& nor, const Vector3f& tc) :
         color(col), normal(nor), tex_coords(tc) {}
};

struct Light
{
    Vector3f position;
    Vector3f intensity;
};