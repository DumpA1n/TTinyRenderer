#pragma once

struct vertex_shader_payload {
    Vector3f position;
};

struct fragment_shader_payload {
    Vector3f pos;
    Vector3f color;
};
