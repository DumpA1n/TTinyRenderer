#include <iostream>

#include "rasterizer.h"

#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <chrono>

long long get_time() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

Matrix4f get_model_matrix() {
    return Matrix4f::identity();
}


int main() {
    Rasterizer rst(700, 700);
    rst.clear_buffer();

    while (1) {
        rst.clear_buffer();
        for (int i = 1; i < 700; i++) {
            for (int j = 1; j < 700; j++) {
                float r = i / 700.0f;
                float g = j / 700.0f;
                float b = 0.5f * (sin(get_time()*0.001f) + 1.0f);
                rst.set_pixel(Point{(float)i, (float)j}, Vector3f{r, g, b});
            }
        }
        stbi_write_png("out.png", 700, 700, 3, rst.get_frame_buffer().data(), 0);
    }

    std::vector<Vector3f> points{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    stbi_write_png("out.png", 700, 700, 3, rst.get_frame_buffer().data(), 0);

    std::cout << "done" << std::endl;
    return 0;
}