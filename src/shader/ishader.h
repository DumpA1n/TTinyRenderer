#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <cmath>
#include <vector>
#include <memory>

#include "texture/texture.h"
#include "utils/mmath.h"

class Scene;
class Object;
class Light;

struct vertex_shader_payload_i {
    std::shared_ptr<Object> object;
    Vector4f position;
    Vector3f normal;
    const Matrix4f& model;
    const Matrix4f& view;
    const Matrix4f& projection;
};

struct vertex_shader_output_i {
    Vector4f position;      // 输出：变换后的位置
    Vector3f normal;        // 输出：变换后的法线
    Vector3f view_pos;      // 输出：观察空间位置
};

struct fragment_shader_payload_i {
    std::shared_ptr<Scene> scene;
    std::shared_ptr<Object> object;
    Vector3f view_pos;
    Vector3f normal;
    Vector2f tex_coord;
    Vector3f color;
};

struct fragment_shader_output_i {
    Vector3f color;         // 输出：片元颜色
};

class IShader {
public:
    IShader() = default;
    virtual ~IShader() = default;

    void initialize() {

    }

    virtual vertex_shader_output_i vertex_shader(const vertex_shader_payload_i& payload) = 0;
    virtual fragment_shader_output_i fragment_shader(const fragment_shader_payload_i& payload) = 0;

    const std::shared_ptr<Texture> get_texture(const std::string& texname) const;

private:
    Matrix4f view_matrix_;
    Matrix4f projection_matrix_;
    Matrix4f model_matrix_;
};
