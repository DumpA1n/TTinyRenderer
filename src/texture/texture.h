#pragma once

#include <memory>
#include <optional>
#include <string>

#include "utils/mmath.h"

class Texture {
public:
    Texture(const std::string& filename);

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    unsigned char* source() const { return image_; }
    int width() const { return width_; }
    int height() const { return height_; }
    int channels() const { return channels_; }

    Vector3f sampler2D(const float u, const float v) const;
    Vector3f sampler2D(const Vector2f uv) const;

private:
    unsigned char* image_;
    int width_, height_, channels_;
};
