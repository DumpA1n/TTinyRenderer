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
    Vector3f sampler2D(float u, float v);
    Vector3f sampler2D(Vector2f uv);
};