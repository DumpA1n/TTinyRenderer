#include "scene.h"

#include "camera/camera_manager.h"
#include "object/camera.h"
#include "shader/default_shader.h"
#include <memory>
#include <mutex>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

void Scene::tick() {
    auto* rst = rasterizer_.get();
    if (rst == nullptr) {
        return;
    }

    auto shader = std::make_unique<DefaultShader>();

    while (true) {
        rst->clear_buffer({0.0f, 0.0f, 0.0f});

        rst->run();
        // rst->run_multi_thread();

        // Vector4f zero  = Vector4f{Vector3f{0, 0, 0}, 1};
        // Vector4f xAxis = Vector4f{normalized(Vector3f{1, 0, 0}), 1};
        // Vector4f yAxis = Vector4f{normalized(Vector3f{0, 1, 0}), 1};
        // Vector4f zAxis = Vector4f{normalized(Vector3f{0, 0, 1}), 1};
        // Vector4f l1    = Vector4f{normalized(Vector3f{20, 20, 20}), 1};
        // Vector4f l2    = Vector4f{normalized(Vector3f{-20, 20, 0}), 1};
        // for (auto& it : {std::ref(zero), std::ref(xAxis), std::ref(yAxis), std::ref(zAxis), std::ref(l1), std::ref(l2)}) {
        //     if (shader) {
        //         vertex_shader_payload_i vs_input{
        //             .object = nullptr,
        //             .position = it,
        //             .normal = {},
        //             .model = rst->get_model_matrix(),
        //             .view = rst->get_view_matrix(),
        //             .projection = rst->get_projection_matrix()
        //         };
        //         auto vs_output = shader->vertex_shader(vs_input);
        //         it.get() = vs_output.position;
        //     }
        //     rst->ViewPort(it, rst->width(), rst->height());
        // }

        // rst->draw_line(zero.xy(), xAxis.xy(), {1, 0, 0});
        // rst->draw_line(zero.xy(), yAxis.xy(), {0, 1, 0});
        // rst->draw_line(zero.xy(), zAxis.xy(), {0, 0, 1});
        // rst->draw_line(zero.xy(), l1.xy(), {0.5f, 0.5f, 0.5f});
        // rst->draw_line(zero.xy(), l2.xy(), {1, 1, 1});

        stbi_write_png("out.png", rst->width(), rst->height(), rst->channels(), rst->get_current_frame_buffer().data(), 0);
        break;
    }
}

void Scene::set_rasterizer(Rasterizer&& rst) {
    rasterizer_ = std::make_shared<Rasterizer>(std::move(rst));
    rasterizer_->initialize();
    assert(!weak_from_this().expired() && "Scene must be managed by shared_ptr");
    rasterizer_->set_scene(shared_from_this());
}

void Scene::add_object(const Object::Ptr& obj) {
    // Key steps: load -> categorize -> store
    obj->load();

    if (obj->template is_a<Camera>()) {
        cameras_.push_back(std::dynamic_pointer_cast<Camera>(obj));
    }

    else if (obj->template is_a<Light>()) {
        lights_.push_back(std::dynamic_pointer_cast<Light>(obj));
    }

    else {
        objects_.push_back(obj);
    }

    all_objects_.push_back(obj);
}

Camera::Ptr Scene::camera() const {
    if (cameras_.empty()) {
        return CameraManager::GetInstance().default_camera();
    }
    return cameras_.front();
}

void Scene::set_current_camera(const std::string& name) {
    std::lock_guard<std::mutex> lock(scene_mutex_);

    auto it = std::find_if(cameras_.begin(), cameras_.end(), [&](const Camera::Ptr& camera) {
        return camera->display_name() == name;
    });

    if (it != cameras_.end()) {
        auto camera = *it;
        cameras_.erase(it);
        cameras_.push_front(camera);
    }
}
