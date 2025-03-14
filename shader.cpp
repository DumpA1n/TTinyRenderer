#include "shader.h"
#include "math.h"

#include <iostream>

#define PI 3.14159265358979323846f
inline float degrees_to_radians(float degree) {
    return degree * (PI / 180.0f);
}

Matrix4f get_model_matrix(const Vector3f& scale, const Vector3f& rotate, const Vector3f& translate) {
    float rx = degrees_to_radians(rotate.x);
    float ry = degrees_to_radians(rotate.y);
    float rz = degrees_to_radians(rotate.z);
    float cx = cos(rx), sx = sin(rx);
    float cy = cos(ry), sy = sin(ry);
    float cz = cos(rz), sz = sin(rz);
    return Matrix4f{
        scale.x * (cy * cz), scale.x * (cz * sx * sy - cx * sz), scale.x * (cx * cz * sy + sx * sz), translate.x,
        scale.y * (cy * sz), scale.y * (cx * cz + sx * sy * sz), scale.y * (cx * sy * sz - cz * sx), translate.y,
        scale.z * (-sy)    , scale.z * (cy * sx)               , scale.z * (cx * cy)               , translate.z,
        0.0f               , 0.0f                              , 0.0f                              , 1.0f
    };
}

Matrix4f get_view_matrix(const Vector3f& eye_pos) {
    Vector3f up{0, 1, 0};
    Vector3f target{0, 0, 0};
    Vector3f z_axis = normalized(eye_pos - target);
    Vector3f x_axis = normalized(cross(up, z_axis));
    Vector3f y_axis = cross(z_axis, x_axis);

    return Matrix4f{
        x_axis.x, x_axis.y, x_axis.z, -(x_axis * eye_pos),
        y_axis.x, y_axis.y, y_axis.z, -(y_axis * eye_pos),
        z_axis.x, z_axis.y, z_axis.z, -(z_axis * eye_pos),
        0,        0,        0,        1
    };
}

Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar) {
    eye_fov = degrees_to_radians(eye_fov);
    return Matrix4f{
        1/(tan(eye_fov/2)*aspect_ratio), 0               , 0                         , 0,
        0                              , 1/tan(eye_fov/2), 0                         , 0,
        0                              , 0               , (zNear+zFar)/(zNear-zFar), (2*zNear*zFar)/(zNear-zFar),
        0                              , 0               , -1                        , 0, 
    };
}
float angleY = -25.0f;
Vector4f default_vertex_shader(const vertex_shader_payload& payload) {
    Vector3f angle{5.0f, angleY, 0.0f};
    Vector3f eye_pos{0.0f, 0.0f, 3.0f};

    Matrix4f model = get_model_matrix({1.0f}, angle, {0.0f, 0.0f, 0.0f});
    Matrix4f view = get_view_matrix(eye_pos);
    Matrix4f projection = get_projection_matrix(45.0f, 1.0f, 0.1f, 50.0f);
    Matrix4f mvp = projection * view * model;
    Matrix4f viewmodel = view * model;

    payload.view_pos = (viewmodel * payload.position).xyz();

    payload.normal = normalized((viewmodel.inverse().transpose() * Vector4f{payload.normal, 0.0f}).xyz());

    payload.position = mvp * payload.position;
    payload.position.x /= payload.position.w;
    payload.position.y /= payload.position.w;
    payload.position.z /= payload.position.w;

    return payload.position;
}

Vector3f default_fragment_shader(const fragment_shader_payload& payload) {
    return payload.color;
}

Vector3f texture_fragment_shader(const fragment_shader_payload& payload) {
    Vector3f texture_color{0.0f};
    if (payload.texture != nullptr)
        texture_color = payload.texture->sampler2D(payload.tex_coords.x, payload.tex_coords.y);
    return texture_color;
}

Vector3f african_head_fragment_shader(const fragment_shader_payload& payload) {
    auto textures = payload.textureMap;
    
    Texture* tex_texture  = textures->count("texture")  ? textures->at("texture")  : nullptr;
    Texture* tex_specular = textures->count("specular") ? textures->at("specular") : nullptr;
    Texture* tex_diffuse  = textures->count("diffuse")  ? textures->at("diffuse")  : nullptr;
    Texture* tex_normal   = textures->count("normal")   ? textures->at("normal")   : nullptr;

    static auto sample_or_default = [](Texture* tex, Vector2f coords) {
        return tex ? tex->sampler2D(coords) : Vector3f{0.0f};
    };
    
    Vector3f texture  = sample_or_default(tex_texture, payload.tex_coords);
    Vector3f specular = sample_or_default(tex_specular, payload.tex_coords);
    Vector3f diffuse  = sample_or_default(tex_diffuse, payload.tex_coords);
    Vector3f nm       = sample_or_default(tex_normal, payload.tex_coords);

    Vector3f kd = diffuse;
    Vector3f ks = specular;

    Light light{{5, 5, 5}};
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
    Vector3f light_dir = normalized(light.position - point);
    Vector3f view_dir = normalized(eye_pos - point);
    Vector3f half_vector = normalized(light_dir + view_dir);

    float diff = std::max(normal * light_dir, 0.0f);
    float spec = std::pow(std::max(normal * half_vector, 0.0f), p);

    Vector3f ambient = kd * 0.15f;
    Vector3f diffuse_component = kd * diff;
    Vector3f specular_component = ks * spec;

    Vector3f result_color = ambient + diffuse_component + specular_component;

    result_color = {
        std::min(result_color.x, 1.0f),
        std::min(result_color.y, 1.0f),
        std::min(result_color.z, 1.0f)
    };

    return result_color;
}


Vector3f phong_texture_fragment_shader(const fragment_shader_payload& payload) {
    Vector3f texture_color{0.0f};
    if (payload.texture != nullptr)
        texture_color = payload.texture->sampler2D(payload.tex_coords.x, payload.tex_coords.y);

    Vector3f ka{0.005, 0.005, 0.005};
    Vector3f kd = texture_color;
    Vector3f ks{0.7937, 0.7937, 0.7937};

    Light l1{{20, 20, 20}, {500, 500, 500}};
    Light l2{{-20, 20, 0}, {500, 500, 500}};

    std::vector<Light> lights = {l1, l2};
    Vector3f amb_light_intensity{10, 10, 10};
    Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Vector3f color = payload.color;
    Vector3f point = payload.view_pos;
    Vector3f normal = payload.normal;

    Vector3f result_color = {0, 0, 0};
    
    for (auto& light : lights)
    {
        Vector3f eye_dir = normalized(eye_pos - point);
        Vector3f light_dir = normalized(light.position - point);
        Vector3f normal_dir = normalized(normal);

        Vector3f I = light.intensity;
        float   r2 = squaredNorm(light.position - point);
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

    return result_color;
}

Vector3f phong_fragment_shader(const fragment_shader_payload& payload) {
    Vector3f ka{0.005, 0.005, 0.005};
    Vector3f kd = payload.color;
    Vector3f ks{0.7937, 0.7937, 0.7937};

    Light l1{{20, 20, 20}, {500, 500, 500}};
    Light l2{{-20, 20, 0}, {500, 500, 500}};

    std::vector<Light> lights = {l1, l2};
    Vector3f amb_light_intensity{10, 10, 10};
    Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Vector3f color = payload.color;
    Vector3f point = payload.view_pos;
    Vector3f normal = payload.normal;

    Vector3f result_color = {0, 0, 0};
    
    for (auto& light : lights)
    {
        Vector3f eye_dir = normalized(eye_pos - point);
        Vector3f light_dir = normalized(light.position - point);
        Vector3f normal_dir = normalized(normal);

        Vector3f I = light.intensity;
        float   r2 = squaredNorm(light.position - point);
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

    return result_color;
}
