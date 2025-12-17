#pragma once

#include <string>
#include <memory>

#include "utils/mmath.h"

class Scene;
class Object;

struct vertex_shader_payload {
    std::shared_ptr<Object> object;
    Vector4f position;
    Vector3f normal;
    const Matrix4f& model;
    const Matrix4f& view;
    const Matrix4f& projection;
};

struct vertex_shader_output {
    Vector4f position;      // 变换后的位置
    Vector3f normal;        // 变换后的法线
    Vector3f view_pos;      // 观察空间位置
};

struct fragment_shader_payload {
    std::shared_ptr<Scene> scene;    // get lights from scene
    std::shared_ptr<Object> object;  // get textures from object
    Vector3f view_pos;
    Vector3f normal;
    Vector2f tex_coord;
    Vector3f color;
};

struct fragment_shader_output {
    Vector3f color;
};

class IShader {
public:
    IShader() = default;
    virtual ~IShader() = default;

    void initialize() {

    }

    virtual vertex_shader_output vertex_shader(const vertex_shader_payload& payload) = 0;

    virtual fragment_shader_output fragment_shader(const fragment_shader_payload& payload) = 0;

protected:
    Vector3f sampler2D(const fragment_shader_payload& payload, const std::string& tex_name) const;

private:
    Matrix4f view_matrix_;
    Matrix4f projection_matrix_;
    Matrix4f model_matrix_;
};
