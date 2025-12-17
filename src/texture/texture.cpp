#include "texture/texture.h"

#include <algorithm>

#include "utils/logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

Texture::Texture(const std::string& filename)
    : image_(nullptr), width_(0), height_(0), channels_(0)
{
    image_ = stbi_load(filename.c_str(), &width_, &height_, &channels_, 0);
    LOGI("Texture loaded: {}, width: {}, height: {}, channels: {}\n", filename, width_, height_, channels_);
}

Vector3f Texture::sampler2D(const float u, const float v) const
{
    if (image_ == nullptr) {
        return Vector3f{0.0f, 0.0f, 0.0f};
    }
    int tex_x = std::clamp(static_cast<int>(u * width_), 0, width_ - 1);
    int tex_y =  std::clamp(static_cast<int>(v * height_), 0, height_ - 1);
    int index = (tex_y * width_ + tex_x) * channels_;
    return Vector3f{
        image_[index] / 255.0f,
        image_[index + 1] / 255.0f,
        image_[index + 2] / 255.0f
    };
}

Vector3f Texture::sampler2D(const Vector2f uv) const
{
    if (image_ == nullptr) {
        return Vector3f{0.0f, 0.0f, 0.0f};
    }
    int tex_x = std::clamp(static_cast<int>(uv.x * width_), 0, width_ - 1);
    int tex_y =  std::clamp(static_cast<int>(uv.y * height_), 0, height_ - 1);
    int index = (tex_y * width_ + tex_x) * channels_;
    return Vector3f{
        image_[index] / 255.0f,
        image_[index + 1] / 255.0f,
        image_[index + 2] / 255.0f
    };
}
