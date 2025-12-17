#pragma once

#include "default_shader.h"

class TextureShader : public DefaultShader {
public:
    TextureShader() = default;

    virtual vertex_shader_output vertex_shader(const vertex_shader_payload& payload) override {
        return DefaultShader::vertex_shader(payload);
    }

    virtual fragment_shader_output fragment_shader(const fragment_shader_payload& payload) override {
        fragment_shader_output output;
        output.color = sampler2D(payload, "texture");
        return output;
    }
};
