#include "shader.h"
#include "math.h"

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
    Vector3f up(0, 1, 0);
    Vector3f target(0, 0, 0);
    Vector3f z_axis = (eye_pos - target).normalized();
    Vector3f x_axis = up.cross(z_axis).normalized();
    Vector3f y_axis = z_axis.cross(x_axis);

    return Matrix4f{
        x_axis.x, x_axis.y, x_axis.z, -x_axis.dot(eye_pos),
        y_axis.x, y_axis.y, y_axis.z, -y_axis.dot(eye_pos),
        z_axis.x, z_axis.y, z_axis.z, -z_axis.dot(eye_pos),
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

Vector4f default_vertex_shader(const vertex_shader_payload& payload) {
    Vector3f angle{0.0f, 140.0f, 0.0f};
    Vector3f eye_pos{0.0f, 0.0f, 10.0f};

    Matrix4f model = get_model_matrix({2.5f}, angle, {0.0f, 0.0f, 0.0f});
    Matrix4f view = get_view_matrix(eye_pos);
    Matrix4f projection = get_projection_matrix(45.0f, 1.0f, 0.1f, 50.0f);
    Matrix4f mvp = projection * view * model;
    Matrix4f viewmodel = view * model;

    payload.view_pos = (viewmodel * payload.position).xyz();

    payload.normal = (viewmodel.inverse().transpose() * Vector4f{payload.normal, 0.0f}).xyz();

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
        texture_color = payload.texture->get_texture_color(payload.tex_coords.x, payload.tex_coords.y);
    return texture_color;
}
#include <iostream>
Vector3f phong_texture_fragment_shader(const fragment_shader_payload& payload) {
    Vector3f texture_color{0.0f};
    if (payload.texture != nullptr)
        texture_color = payload.texture->get_texture_color(payload.tex_coords.x, payload.tex_coords.y);

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
        Vector3f eye_dir = (eye_pos - point).normalized();
        Vector3f light_dir = (light.position - point).normalized();
        Vector3f normal_dir = normal.normalized();

        Vector3f I = light.intensity;
        float   r2 = (light.position - point).squaredNorm();
        Vector3f h = (eye_dir + light_dir).normalized();

        Vector3f ambient = ka.cwiseProduct(amb_light_intensity);

        Vector3f diffuse = kd.cwiseProduct(I / r2) * std::max(0.0f, normal_dir.dot(light_dir));

        Vector3f specular = ks.cwiseProduct(I / r2) * std::max(0.0f, std::pow(normal_dir.dot(h), p));

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
        Vector3f eye_dir = (eye_pos - point).normalized();
        Vector3f light_dir = (light.position - point).normalized();
        Vector3f normal_dir = normal;

        Vector3f I = light.intensity;
        float   r2 = (light.position - point).squaredNorm();
        Vector3f h = (eye_dir + light_dir).normalized();

        Vector3f ambient = ka.cwiseProduct(amb_light_intensity);

        Vector3f diffuse = kd.cwiseProduct(I / r2) * std::max(0.0f, normal_dir.dot(light_dir));

        Vector3f specular = ks.cwiseProduct(I / r2) * std::max(0.0f, std::pow(normal_dir.dot(h), p));

        result_color += ambient + diffuse + specular;
    }

    return result_color;
}