#pragma once

#include <vector>
#include <string>

#include "mmath.h"

class Texture {
private:
    int width, height, channels;

    unsigned char* image;

public:
    Texture(const std::string& filename);
    Vector3f get_texture_color(float u, float v);
};