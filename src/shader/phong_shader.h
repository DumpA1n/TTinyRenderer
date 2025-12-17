#pragma once

#include "texture_shader.h"

class PhongShader : public TextureShader {
public:
    PhongShader() = default;

    virtual vertex_shader_output vertex_shader(const vertex_shader_payload& payload) override;

    virtual fragment_shader_output fragment_shader(const fragment_shader_payload& payload) override;
};
