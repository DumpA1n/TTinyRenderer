#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "texture.h"
#include "utils/Singleton.h"

class TextureManager : public Singleton<TextureManager> {
    friend class Singleton<TextureManager>;

private:
    TextureManager() = default;
    ~TextureManager() = default;

public:
    void initialize() {
        add_texture("af_texture", "res/models/african_head/african_head_SSS.jpg");
        add_texture("af_diffuse", "res/models/african_head/african_head_diffuse.tga");
        add_texture("af_specular", "res/models/african_head/african_head_spec.tga");
        add_texture("af_normal", "res/models/african_head/african_head_nm_tangent.tga");
        add_texture("spot_texture", "res/models/spot/spot_texture.png");
        add_texture("diablo3_texture", "res/models/diablo3_pose/diablo3_pose_diffuse.tga");
    }

    void add_texture(const std::string& texname, const std::string& filename) {
        if (textures_.find(texname) == textures_.end()) {
            textures_[texname] = std::make_shared<Texture>(filename);
        }
    }

    const std::shared_ptr<Texture> get_texture(const std::string& texname) const {
        if (textures_.find(texname) != textures_.end()) {
            return textures_.at(texname);
        }
        return nullptr;
    }

    auto& textures() const {
        return textures_;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures_;
};
