#include <iostream>
#include <memory>
#include <chrono>

#include "object/object.h"
#include "rasterizer/rasterizer.h"
#include "scene/scene_manager.h"
#include "shader/africahead_shader.h"
#include "shader/phong_shader.h"
#include "texture/texture_manager.h"
#include "utils/logger.h"
#include "utils/mmath.h"

int main() {
    SceneManager::GetInstance().initialize();
    auto scene = SceneManager::GetInstance().get_scene("default");

    scene->set_rasterizer(Rasterizer(700, 700, 4));

    auto& texmgr = TextureManager::GetInstance();
    texmgr.initialize();

    ModelObject african_head("res/models/african_head/african_head.obj");
    african_head.set_position({-1.0f, -1.0f, -1.0f});
    african_head.set_rotation({0.0f, 10.0f, 0.0f});
    african_head.set_scale({1.0f, 1.0f, 1.0f});
    african_head.set_shader(std::make_unique<AfricaHeadShader>());
    african_head.add_texture("texture", texmgr.get_texture("af_texture"));
    african_head.add_texture("diffuse", texmgr.get_texture("af_diffuse"));
    african_head.add_texture("specular", texmgr.get_texture("af_specular"));
    african_head.add_texture("normal", texmgr.get_texture("af_normal"));

    ModelObject spot("res/models/spot/spot_triangulated_good.obj");
    spot.set_position({0.0f, 0.0f, 0.0f});
    spot.set_rotation({0.0f, 160.0f, 0.0f});
    spot.set_scale({1.0f, 1.0f, 1.0f});
    spot.set_display_name("Spot");
    spot.set_shader(std::make_unique<PhongShader>());
    spot.add_texture("texture", texmgr.get_texture("spot_texture"));

    ModelObject diablo3_pose("res/models/diablo3_pose/diablo3_pose.obj");
    diablo3_pose.set_position({1.0f, 1.0f, 1.0f});
    diablo3_pose.set_rotation({0.0f, 10.0f, 0.0f});
    diablo3_pose.set_scale({1.0f, 1.0f, 1.0f});
    diablo3_pose.set_display_name("Diablo3Pose");
    diablo3_pose.set_shader(std::make_unique<PhongShader>());
    diablo3_pose.add_texture("texture", texmgr.get_texture("diablo3_texture"));

    scene->add_object(Eye(Vector3f{0, 0, 7}));
    scene->add_object(Light(Vector3f{20, 20, 20}, Vector3f{500, 500, 500}));
    scene->add_object(Light(Vector3f{-20, 20, 0}, Vector3f{500, 500, 500}));
    scene->add_object(std::move(african_head));
    scene->add_object(std::move(spot));
    scene->add_object(std::move(diablo3_pose));

    auto start = std::chrono::high_resolution_clock::now(); // 计算耗时

    scene->tick();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    LOGI("Takes Time: {} ms\n", elapsed.count());

    return 0;
}