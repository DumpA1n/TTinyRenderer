#include "phong_shader.h"

#include "object/object.h"
#include "scene/scene.h"

vertex_shader_output PhongShader::vertex_shader(const vertex_shader_payload& payload) {
    return TextureShader::vertex_shader(payload);
}

fragment_shader_output PhongShader::fragment_shader(const fragment_shader_payload& payload) {
    fragment_shader_output output;

    Vector3f ka{0.005, 0.005, 0.005};
    Vector3f kd = sampler2D(payload, "texture");
    Vector3f ks{0.7937, 0.7937, 0.7937};

    Vector3f amb_light_intensity{10, 10, 10};
    Vector3f eye_pos = payload.scene->camera()->position();

    float p = 150;

    Vector3f point = payload.view_pos;
    Vector3f normal = payload.normal;

    Vector3f result_color = {0, 0, 0};

    for (const auto &light : payload.scene->lights()) {
        Vector3f eye_dir = normalized(eye_pos - point);
        Vector3f light_dir = normalized(light->position() - point);
        Vector3f normal_dir = normalized(normal);

        Vector3f I = light->intensity();
        float r2 = squaredNorm(light->position() - point);
        Vector3f h = normalized(eye_dir + light_dir);

        Vector3f ambient = cwise(ka, amb_light_intensity);
        Vector3f diffuse = cwise(kd, I / r2) * std::max(0.0f, dot(normal_dir, light_dir));
        Vector3f specular = cwise(ks, I / r2) * std::max(0.0f, std::pow(dot(normal_dir, h), p));

        result_color += ambient + diffuse + specular;
    }

    for (int i : {0, 1, 2}) {
      result_color[i] = std::min(result_color[i], 1.0f);
    }

    output.color = result_color;

    return output;
}
