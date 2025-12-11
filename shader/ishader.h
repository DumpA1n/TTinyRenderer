#pragma once

#include "texture.h"
#include "mmath.h"

#include <string>
#include <unordered_map>
#include <cmath>
#include <vector>

struct vertex_shader_payload_i {
    Vector4f position;
    Vector3f normal;
};

struct vertex_shader_output_i {
    Vector4f position;      // 输出：变换后的位置
    Vector3f normal;        // 输出：变换后的法线
    Vector3f view_pos;      // 输出：观察空间位置
};

struct fragment_shader_payload_i {
    Vector3f view_pos;
    Vector3f normal;
    Vector2f tex_coord;
    Vector3f color;
};

struct fragment_shader_output_i {
    Vector3f color;         // 输出：片元颜色
};

struct Light {
    Vector3f position;
    Vector3f intensity;
};

class IShader {
public:
    IShader() = default;
    virtual ~IShader() = default;

    void initialize() {

    }

    virtual vertex_shader_output_i vertex_shader(const vertex_shader_payload_i& payload) = 0;
    virtual fragment_shader_output_i fragment_shader(const fragment_shader_payload_i& payload) = 0;

    inline float degrees_to_radians(float degree) {
        return degree * (M_PI / 180.0f);
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

    void set_model_matrix(const Matrix4f& model) {
        model_matrix_ = model;
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

    void set_view_matrix(const Matrix4f& view) {
        view_matrix_ = view;
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

    void set_projection_matrix(const Matrix4f& projection) {
        projection_matrix_ = projection;
    }

    auto& textures() {
        return textures_;
    }
    void add_texture(const std::string& name, const Texture& texture) {
        textures_.emplace(name, texture);
    }

private:
    std::unordered_map<std::string, Texture> textures_;

    std::vector<Light> lights_;

    Matrix4f view_matrix_;
    Matrix4f projection_matrix_;
    Matrix4f model_matrix_;
};
