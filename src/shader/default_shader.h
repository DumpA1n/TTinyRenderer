#pragma once

#include "ishader.h"

class DefaultShader : public IShader {
public:
    DefaultShader() = default;

    virtual vertex_shader_output_i vertex_shader(const vertex_shader_payload_i& payload) override {
        float angleY = -25.0f;
        Vector3f angle{5.0f, angleY, 0.0f};
        Vector3f eye_pos{0.0f, 0.0f, 3.0f};

        Matrix4f mvp = payload.projection * payload.view * payload.model;
        Matrix4f viewmodel = payload.view * payload.model;

        vertex_shader_output_i output;

        // Compute view position
        output.view_pos = (viewmodel * payload.position).xyz();

        // Transform normal with the inverse transpose of the viewmodel matrix
        output.normal = normalized((viewmodel.inverse().transpose() * Vector4f{payload.normal, 0.0f}).xyz());

        // NDC coordinates
        Vector4f position = mvp * payload.position;
        position.x /= position.w;
        position.y /= position.w;
        position.z /= position.w;

        output.position = position;

        return output;
    }

    virtual fragment_shader_output_i fragment_shader(const fragment_shader_payload_i& payload) override {
        fragment_shader_output_i output;

        output.color = payload.color;

        return output;
    }

};
