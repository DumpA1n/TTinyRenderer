#pragma once

#include <string>

#include "scene.h"
#include "utils/Singleton.h"

class SceneManager : public Singleton<SceneManager> {
    friend class Singleton<SceneManager>;

public:
    void initialize() {
        add_scene("default");
    }

    void add_scene(const std::string& name) {
        if (scenes_.find(name) == scenes_.end()) {
            scenes_[name] = std::make_shared<Scene>();
        }
    }

    std::shared_ptr<Scene> get_scene(const std::string& name) const {
        if (scenes_.find(name) != scenes_.end()) {
            return scenes_.at(name);
        }
        return nullptr;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Scene>> scenes_;
};
