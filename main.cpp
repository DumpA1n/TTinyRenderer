#include <memory>
#include <chrono>

#include "camera/camera_manager.h"
#include "object/object_manager.h"
#include "rasterizer/rasterizer.h"
#include "scene/scene_manager.h"
#include "texture/texture_manager.h"
#include "utils/logger.h"
#include "utils/mmath.h"

int main() {
    // Initialize managers
    TextureManager::GetInstance().initialize();
    ObjectManager::GetInstance().initialize(); // Depends on TextureManager
    CameraManager::GetInstance().initialize();
    SceneManager::GetInstance().initialize();

    // Setup scene
    auto scene = SceneManager::GetInstance().get_scene("default");

    scene->set_rasterizer(Rasterizer(700, 700, 4));

    scene->add_objects(ObjectManager::GetInstance().objects());
    scene->add_object(CameraManager::GetInstance().default_camera());
    scene->add_object(Light(Vector3f{20, 20, 20}, Vector3f{500, 500, 500}));
    scene->add_object(Light(Vector3f{-20, 20, 0}, Vector3f{500, 500, 500}));

    auto start = std::chrono::high_resolution_clock::now();

    // Main loop
    scene->tick();

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end - start;
    LOGI("Takes Time: {} ms\n", elapsed.count());

    return 0;
}