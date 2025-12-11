#pragma once

#include "ishader.h"
#include "mmath.h"
#include <algorithm>

class TextureShader : public IShader {
public:
    TextureShader() = default;

    vertex_shader_output_i vertex_shader(const vertex_shader_payload_i& payload) override {
        float angleY = -25.0f;
        Vector3f angle{5.0f, angleY, 0.0f};
        Vector3f eye_pos{0.0f, 0.0f, 3.0f};

        Matrix4f model = get_model_matrix({1.0f}, angle, {0.0f, 0.0f, 0.0f});
        Matrix4f view = get_view_matrix(eye_pos);
        Matrix4f projection = get_projection_matrix(45.0f, 1.0f, 0.1f, 50.0f);
        Matrix4f mvp = projection * view * model;
        Matrix4f viewmodel = view * model;

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

    fragment_shader_output_i fragment_shader(const fragment_shader_payload_i& payload) override {
        fragment_shader_output_i output;

        Vector3f texture_color{0.0f};
        if (textures().count("texture")) {
            texture_color = textures().at("texture").sampler2D(payload.tex_coord.x, payload.tex_coord.y);
        }

        Vector3f ka{0.005, 0.005, 0.005};
        Vector3f kd = texture_color;
        Vector3f ks{0.7937, 0.7937, 0.7937};

        Light l1{{20, 20, 20}, {500, 500, 500}};
        Light l2{{-20, 20, 0}, {500, 500, 500}};

        std::vector<Light> lights = {l1, l2};
        Vector3f amb_light_intensity{10, 10, 10};
        Vector3f eye_pos{0, 0, 10};

        float p = 150;

        Vector3f point = payload.view_pos;
        Vector3f normal = payload.normal;

        Vector3f result_color = {0, 0, 0};
        
        for (auto& light : lights) {
            Vector3f eye_dir = normalized(eye_pos - point);
            Vector3f light_dir = normalized(light.position - point);
            Vector3f normal_dir = normalized(normal);

            Vector3f I = light.intensity;
            float r2 = squaredNorm(light.position - point);
            Vector3f h = normalized(eye_dir + light_dir);

            Vector3f ambient = cwiseProduct(ka, amb_light_intensity);
            Vector3f diffuse = cwiseProduct(kd, I / r2) * std::max(0.0f, normal_dir * light_dir);
            Vector3f specular = cwiseProduct(ks, I / r2) * std::max(0.0f, std::pow(normal_dir * h, p));

            result_color += ambient + diffuse + specular;
        }

        result_color = {
            result_color.x > 1.0f ? 1.0f : result_color.x,
            result_color.y > 1.0f ? 1.0f : result_color.y,
            result_color.z > 1.0f ? 1.0f : result_color.z
        };

        output.color = result_color;

        return output;
    }
};
