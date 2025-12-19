#include "africahead_shader.h"

#include "object/object.h"

vertex_shader_output AfricaHeadShader::vertex_shader(const vertex_shader_payload& payload) {
    return TextureShader::vertex_shader(payload);
}

fragment_shader_output AfricaHeadShader::fragment_shader(const fragment_shader_payload& payload) {
    fragment_shader_output output;

    Vector3f texture  = sampler2D(payload, "texture");
    Vector3f specular = sampler2D(payload, "specular");
    Vector3f diffuse  = sampler2D(payload, "diffuse");
    Vector3f nm       = sampler2D(payload, "normal");

    Vector3f kd = diffuse;
    Vector3f ks = specular;

    Light light{{5, 5, 5}, {500, 500, 500}};
    Vector3f eye_pos{0, 0, 10};
    float p = 150;

    Vector3f point = payload.view_pos;
    Vector3f normal = payload.normal;

    // 构建TBN矩阵
    Vector3f n = normalized(normal);
    float x = n.x, y = n.y, z = n.z;
    Vector3f t{x*y / std::sqrt(x*x + z*z), std::sqrt(x*x + z*z), z*y / std::sqrt(x*x + z*z)};
    Vector3f b = cross(n, (t));
    Matrix3f TBN{ t, b, n };

    // 法线贴图值从[0,1]映射到[-1,1]
    nm = nm * 2.0f - Vector3f{1.0f};
    normal = normalized(TBN * nm);

    // 计算光照
    Vector3f light_dir = normalized(light.position() - point);
    Vector3f view_dir = normalized(eye_pos - point);
    Vector3f half_vector = normalized(light_dir + view_dir);

    float diff = std::max(dot(normal, light_dir), 0.0f);
    float spec = std::pow(std::max(dot(normal, half_vector), 0.0f), p);

    Vector3f ambient = kd * 0.15f;
    Vector3f diffuse_component = kd * diff;
    Vector3f specular_component = ks * spec;

    Vector3f result_color = ambient + diffuse_component + specular_component;

    for (int i : {0, 1, 2}) {
        result_color[i] = std::min(result_color[i], 1.0f);
    }

    output.color = result_color;

    return output;
}
