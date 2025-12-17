#include "ishader.h"

#include "object/object.h"
#include "texture/texture.h"

Vector3f IShader::sampler2D(const fragment_shader_payload& payload, const std::string& tex_name) const {
    if (payload.object->textures().contains(tex_name)) {
        return payload.object->textures().at(tex_name)->sampler2D(payload.tex_coord);
    }
    return Vector3f{0.0f, 0.0f, 0.0f};
};
