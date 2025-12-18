#include "object_manager.h"

#include "shader/africahead_shader.h"
#include "shader/phong_shader.h"
#include "texture/texture_manager.h"

void ObjectManager::initialize() {
    auto& tex_mgr = TextureManager::GetInstance();

    ModelObject african_head("res/models/african_head/african_head.obj");
    african_head.set_position({-1.0f, -1.0f, -1.0f});
    african_head.set_rotation({0.0f, 10.0f, 0.0f});
    african_head.set_scale({1.0f, 1.0f, 1.0f});
    african_head.set_shader(std::make_unique<AfricaHeadShader>());
    african_head.add_texture("texture", tex_mgr.get_texture("af_texture"));
    african_head.add_texture("diffuse", tex_mgr.get_texture("af_diffuse"));
    african_head.add_texture("specular", tex_mgr.get_texture("af_specular"));
    african_head.add_texture("normal", tex_mgr.get_texture("af_normal"));

    ModelObject spot("res/models/spot/spot_triangulated_good.obj");
    spot.set_position({0.0f, 0.0f, 0.0f});
    spot.set_rotation({0.0f, 160.0f, 0.0f});
    spot.set_scale({1.0f, 1.0f, 1.0f});
    spot.set_display_name("Spot");
    spot.set_shader(std::make_unique<PhongShader>());
    spot.add_texture("texture", tex_mgr.get_texture("spot_texture"));

    ModelObject diablo3_pose("res/models/diablo3_pose/diablo3_pose.obj");
    diablo3_pose.set_position({1.0f, 1.0f, 1.0f});
    diablo3_pose.set_rotation({0.0f, 10.0f, 0.0f});
    diablo3_pose.set_scale({1.0f, 1.0f, 1.0f});
    diablo3_pose.set_display_name("Diablo3Pose");
    diablo3_pose.set_shader(std::make_unique<PhongShader>());
    diablo3_pose.add_texture("texture", tex_mgr.get_texture("diablo3_texture"));

    objects_.push_back(std::make_shared<ModelObject>(std::move(african_head)));
    objects_.push_back(std::make_shared<ModelObject>(std::move(spot)));
    objects_.push_back(std::make_shared<ModelObject>(std::move(diablo3_pose)));
}
