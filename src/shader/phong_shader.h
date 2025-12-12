#pragma once

#include "texture_shader.h"

#include "object/object.h"
#include "scene/scene.h"

class PhongShader : public TextureShader {
public:
    PhongShader() = default;

    virtual vertex_shader_output_i vertex_shader(const vertex_shader_payload_i& payload) override {
        return TextureShader::vertex_shader(payload);
    }

    virtual fragment_shader_output_i fragment_shader(const fragment_shader_payload_i& payload) override {
        fragment_shader_output_i output;

        Vector3f ka{0.005, 0.005, 0.005};
        Vector3f kd = payload.object->textures().at("texture")->sampler2D(payload.tex_coord);
        Vector3f ks{0.7937, 0.7937, 0.7937};

        Vector3f amb_light_intensity{10, 10, 10};
        Vector3f eye_pos{0, 0, 10};

        float p = 150;

        Vector3f point = payload.view_pos;
        Vector3f normal = payload.normal;

        Vector3f result_color = {0, 0, 0};

        for (const auto& light : payload.scene->lights()) {
            Vector3f eye_dir = normalized(eye_pos - point);
            Vector3f light_dir = normalized(light->position() - point);
            Vector3f normal_dir = normalized(normal);

            Vector3f I = light->intensity();
            float r2 = squaredNorm(light->position() - point);
            Vector3f h = normalized(eye_dir + light_dir);

            Vector3f ambient = cwiseProduct(ka, amb_light_intensity);
            Vector3f diffuse = cwiseProduct(kd, I / r2) * std::max(0.0f, normal_dir * light_dir);
            Vector3f specular = cwiseProduct(ks, I / r2) * std::max(0.0f, std::pow(normal_dir * h, p));

            result_color += ambient + diffuse + specular;
        }

        for (int i : {0, 1, 2}) {
            result_color[i] = std::min(result_color[i], 1.0f);
        }

        output.color = result_color;

        return output;
    }
};