#pragma once

#include "object/eye.h"
#include "object/object.h"
#include "rasterizer/rasterizer.h"
#include <memory>
#include <vector>

class Scene : public std::enable_shared_from_this<Scene> {
public:
    Scene() = default;
    ~Scene() = default;

    void tick();

    void set_rasterizer(Rasterizer&& rst) {
        rasterizer_ = std::make_shared<Rasterizer>(std::move(rst));
        rasterizer_->initialize();
        rasterizer_->set_scene(shared_from_this());
    }

    template<typename T>
    void add_object(T&& obj) {
        static_assert(std::is_base_of_v<Object, std::decay_t<T>>, "T must derive from Object");

        auto ptr = std::make_shared<std::decay_t<T>>(std::forward<T>(obj));

        ptr->load();

        if (ptr->template is_a<Eye>()) {
            eye_ = std::dynamic_pointer_cast<Eye>(ptr);
        }
        else if (ptr->template is_a<Light>()) {
            lights_.push_back(std::dynamic_pointer_cast<Light>(ptr));
        }
        else {
            objects_.push_back(ptr);
        }

        all_objects_.push_back(ptr);
    }

    auto& objects() const { return objects_; }
    auto& lights() const { return lights_; }
    auto& eye() const { return eye_; }

private:
    std::shared_ptr<Rasterizer> rasterizer_ = nullptr;

    std::vector<std::shared_ptr<Object>> all_objects_;
    std::vector<std::shared_ptr<Object>> objects_;
    std::vector<std::shared_ptr<Light>> lights_;
    std::shared_ptr<Eye> eye_;
};
