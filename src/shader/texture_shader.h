#pragma once

#include <algorithm>

#include "default_shader.h"

class TextureShader : public DefaultShader {
public:
    TextureShader() = default;

    virtual vertex_shader_output_i vertex_shader(const vertex_shader_payload_i& payload) override {
        return DefaultShader::vertex_shader(payload);
    }

    virtual fragment_shader_output_i fragment_shader(const fragment_shader_payload_i& payload) override {
        fragment_shader_output_i output;
        output.color = get_texture("texture")->sampler2D(payload.tex_coord);
        return output;
    }
};
