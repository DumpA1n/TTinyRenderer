#pragma once

#include <list>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

#include "object/camera.h"
#include "object/object.h"
#include "rasterizer/rasterizer.h"

class Scene : public std::enable_shared_from_this<Scene> {
public:
    Scene() = default;
    ~Scene() = default;

    void tick();

    void set_rasterizer(Rasterizer&& rst);

    void add_object(const Object::Ptr& obj);

    template <typename T,
              std::enable_if_t<std::is_base_of_v<Object, std::decay_t<T>>, int> = 0>
    void add_object(T&& obj) {
        add_object(std::make_shared<std::decay_t<T>>(std::forward<T>(obj)));
    }

    void add_objects(const std::vector<Object::Ptr>& objs) {
        for (const auto& obj : objs) {
            add_object(obj);
        }
    }

    auto& all_objects() const { return all_objects_; }
    auto& objects() const { return objects_; }
    auto& lights() const { return lights_; }
    auto& cameras() const { return cameras_; }

    Camera::Ptr camera() const;
    void set_current_camera(const std::string& name);

private:
    std::mutex scene_mutex_;

    std::shared_ptr<Rasterizer> rasterizer_ = nullptr;

    std::vector<std::shared_ptr<Object>> all_objects_;
    std::vector<std::shared_ptr<Object>> objects_;
    std::vector<std::shared_ptr<Light>> lights_;
    std::list<std::shared_ptr<Camera>> cameras_;
};
