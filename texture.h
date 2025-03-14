#pragma once

#include <vector>
#include <string>

#include "mmath.h"

class Texture {
private:
    unsigned char* image;

public:
    int width, height, channels;

    Texture(const std::string& filename);
    Vector3f sampler2D(const float& u, const float& v);
    Vector3f sampler2D(const Vector2f& uv);
};