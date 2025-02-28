#include <iostream>

#include "rasterizer.h"

#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <chrono>

#define WIDTH 700
#define HEIGHT 700

long long get_time() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

Matrix4f get_model_matrix() {
    return Matrix4f::identity();
}


int main() {
    Rasterizer rst(WIDTH, HEIGHT);
    rst.clear_buffer();

    // while (1) {
    //     rst.clear_buffer();
    //     for (int i = 0; i < WIDTH; i++) {
    //         for (int j = 0; j < HEIGHT; j++) {
    //             float r = i / WIDTH;
    //             float g = j / HEIGHT;
    //             float b = 0.5f * (sin(get_time()*0.001f) + 1.0f);
    //             rst.set_pixel(Vector3f{(float)i, (float)j}, Vector3f{r, g, b});
    //         }
    //     }
    //     stbi_write_png("out.png", WIDTH, HEIGHT, 3, rst.get_stb_frame_buffer().data(), WIDTH * 3);
    // }

    // std::vector<Vector3f> Vector3fs{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    Vector3f color{1.0f, 1.0f, 1.0f};
    // rst.set_pixel(Vector3f{0, 0}, color);
    // rst.set_pixel(Vector3f{1, 1}, color);
    // rst.set_pixel(Vector3f{2, 2}, color);
    // rst.set_pixel(Vector3f{3, 3}, color);
    // rst.set_pixel(Vector3f{4, 4}, color);
    // rst.set_pixel(Vector3f{5, 5}, color);
    rst.draw_line(Vector3f{1, 124}, Vector3f{142, 33}, color);
    // std::vector<Vector3f> pos{{123, 12, -123}, {12, 123, -123}, {-123, 12, -123}};
    std::vector<Vector3f> pos{{100, 100}, {50, 50}, {150, 50}};
    rst.draw_triangle(pos, color);
    rst.draw_triangle_fill(pos, color);
    stbi_write_png("out.png", WIDTH, HEIGHT, 3, rst.get_stb_frame_buffer().data(), WIDTH * 3);


    // int width = 700, height = 700;
    // unsigned char *image = (unsigned char *)malloc(width * height * 3); // RGB 图像

    // 生成渐变图像
    // for (int y = 0; y < height; y++) {
    //     for (int x = 0; x < width; x++) {
    //         int index = (y * width + x) * 3;
    //         image[index] = (unsigned char)x;      // 红色通道
    //         image[index + 1] = (unsigned char)y;  // 绿色通道
    //         image[index + 2] = 128;               // 蓝色通道（固定值）
    //     }
    // }

    // 写入 PNG
    // stbi_write_png("out.png", WIDTH, HEIGHT, 3, image, width * 3);

    // free(image);

    std::cout << "done" << std::endl;
    return 0;
}