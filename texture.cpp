#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <algorithm>

Texture::Texture(const std::string& filename) {
    image_ = stbi_load(filename.c_str(), &width_, &height_, &channels_, 0);
    std::cout << "Texture loaded: " << filename << ", width: " << width_ << ", height: " << height_ << ", channels: " << channels_ << std::endl;
}

Vector3f Texture::sampler2D(const float u, const float v) const {
    int tex_x = std::clamp(u * width_, 0.0f, (float)width_);
    int tex_y =  std::clamp(v * height_, 0.0f, (float)height_);
    int index = (tex_y * width_ + tex_x) * channels_;
    return Vector3f{
        image_[index] / 255.0f,
        image_[index + 1] / 255.0f,
        image_[index + 2] / 255.0f
    };
}

Vector3f Texture::sampler2D(const Vector2f uv) const {
    int tex_x = std::clamp(uv.x * width_, 0.0f, (float)width_);
    int tex_y =  std::clamp(uv.y * height_, 0.0f, (float)height_);
    int index = (tex_y * width_ + tex_x) * channels_;
    return Vector3f{
        image_[index] / 255.0f,
        image_[index + 1] / 255.0f,
        image_[index + 2] / 255.0f
    };
}
