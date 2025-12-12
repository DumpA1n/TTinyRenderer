#include "ishader.h"

#include "texture/texture_manager.h"

const std::shared_ptr<Texture> IShader::get_texture(const std::string& texname) const {
    return TextureManager::GetInstance().get_texture(texname);
}
