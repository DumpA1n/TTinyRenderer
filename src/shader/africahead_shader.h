#pragma once

#include "texture_shader.h"

class AfricaHeadShader : public TextureShader {
public:
    AfricaHeadShader() = default;

    vertex_shader_output vertex_shader(const vertex_shader_payload& payload) override;

    fragment_shader_output fragment_shader(const fragment_shader_payload& payload) override;
};
