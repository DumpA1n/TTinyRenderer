#pragma once

#include "texture.h"

#include <string>
#include <unordered_map>

extern float angleY;

struct vertex_shader_payload {
    Vector4f& position;
    Vector3f& normal;
    Vector3f& view_pos;
};

struct fragment_shader_payload {
    Vector3f view_pos;
    Vector3f color;
    Vector3f normal;
    Vector2f tex_coords;
    Texture* texture;
    std::unordered_map<std::string, Texture*> *textureMap;
    fragment_shader_payload(const Vector3f& col, const Vector3f& nor, const Vector2f& tc, Texture* tex, decltype(textureMap) texMap) :
         color(col), normal(nor), tex_coords(tc), texture(tex), textureMap(texMap) {}
};

struct Light
{
    Vector3f position;
    Vector3f intensity;
};

Vector4f default_vertex_shader(const vertex_shader_payload& payload);

Vector3f default_fragment_shader(const fragment_shader_payload& payload);

Vector3f african_head_fragment_shader(const fragment_shader_payload& payload);

Vector3f texture_fragment_shader(const fragment_shader_payload& payload);

Vector3f phong_texture_fragment_shader(const fragment_shader_payload& payload);

Vector3f phong_fragment_shader(const fragment_shader_payload& payload);