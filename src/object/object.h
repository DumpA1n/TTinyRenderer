#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils/mmath.h"
#include "utils/model.h"
#include "utils/triangle.h"
#include "shader/ishader.h"
#include "texture/texture.h"

class Scene;

class Object : public std::enable_shared_from_this<Object> {
public:
    using Ptr = std::shared_ptr<Object>;

    Object(const Vector3f& pos = {0, 0, 0}, const Vector3f& rot = {0, 0, 0}, const Vector3f& s = {1, 1, 1})
        : position_(pos), rotation_(rot), scale_(s) {}

    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;

    Object(Object&&) = default;
    Object& operator=(Object&&) = default;

    virtual ~Object() = default;

    virtual std::string name() = 0;

    template<class T>
    bool is_a() {
        assert(!weak_from_this().expired() && "Object must be managed by shared_ptr");
        return std::dynamic_pointer_cast<T>(shared_from_this()) != nullptr;
    }

    virtual void load();

    auto& shader() const { return shader_; }
    void set_shader(std::unique_ptr<IShader> shader) { shader_ = std::move(shader); }

    auto& textures() const { return textures_; }
    void add_texture(const std::string& name, std::shared_ptr<Texture> texture) { textures_.emplace(name, texture); }

    auto& triangles() const { return triangles_; }

    Vector3f position() const { return position_; }
    void set_position(const Vector3f& pos) { position_ = pos; }

    Vector3f rotation() const { return rotation_; }
    void set_rotation(const Vector3f& rot) { rotation_ = rot; }

    Vector3f scale() const { return scale_; }
    void set_scale(const Vector3f& s) { scale_ = s; }

    std::string display_name() const { return display_name_; }
    void set_display_name(const std::string& name) { display_name_ = name; }

protected:
    std::vector<Triangle> triangles_;

private:
    Vector3f position_;
    Vector3f rotation_;
    Vector3f scale_;

    std::shared_ptr<IShader> shader_;
    std::shared_ptr<Scene> scene_;

    std::unordered_map<std::string, std::shared_ptr<Texture>> textures_;

    std::string display_name_;
};

class ModelObject : public Object {
public:
    using Ptr = std::shared_ptr<ModelObject>;

    explicit ModelObject(const std::string& model_path) : model_path_(model_path) {}

    virtual std::string name() override { return "Model"; }

    virtual void load() override {
        Object::load();

        model_ = std::make_unique<Model>();
        model_->load(model_path_);

        for (int i = 0; i < model_->size(); i += 3) {
            Triangle tri;
            for (int j = 0; j < 3; j++) {
                tri.setVertices(j, model_->getVertices(i + j));
                tri.setTexCoords(j, model_->getTexCoords(i + j));
                tri.setNormals(j, model_->getNormals(i + j));
            }
            triangles_.push_back(tri);
        }
    }

private:
    std::string model_path_;
    std::unique_ptr<Model> model_ = nullptr;
};

class AfricaHeadObject : public ModelObject {
public:
    using Ptr = std::shared_ptr<AfricaHeadObject>;

    explicit AfricaHeadObject(const std::string& model_path) : ModelObject(model_path) {}

    std::string name() override { return "AfricaHead"; }

    virtual void load() override {
        ModelObject::load();
    }
};

class Light : public Object {
public:
    using Ptr = std::shared_ptr<Light>;

    explicit Light(const Vector3f& pos, const Vector3f& intens)
        : Object(pos), intensity_(intens) {}

    std::string name() override { return "Light"; }

    Vector3f intensity() const { return intensity_; }
    void set_intensity(const Vector3f& intens) { intensity_ = intens; }

private:
    Vector3f intensity_;
};
