
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "algorithm"

#include <iostream>

Texture::Texture(const std::string& filename) {
    image = stbi_load(filename.c_str(), &width, &height, &channels, 0);
}

Vector3f Texture::get_texture_color(float u, float v) {
    int tex_x = std::clamp(u * width, 0.0f, (float)width);
    int tex_y =  std::clamp((1 - v) * height, 0.0f, (float)height);
    int index = (tex_y * width + tex_x) * channels;
    return Vector3f{
        image[index] / 255.0f,
        image[index + 1] / 255.0f,
        image[index + 2] / 255.0f
    };
}