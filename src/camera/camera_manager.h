#pragma once

#include <list>
#include <vector>

#include "object/camera.h"
#include "utils/Singleton.h"

class CameraManager : public Singleton<CameraManager> {
    friend class Singleton<CameraManager>;

public:
    void initialize() {
        default_camera_ = std::make_shared<Camera>(Vector3f{0, 0, 7});
        default_camera_->set_display_name("DefaultCamera");
        cameras_.push_back(default_camera_);
    }

    Camera::Ptr default_camera() const {
        return default_camera_;
    }

private:
    Camera::Ptr default_camera_;
    std::vector<Camera::Ptr> cameras_;
};
