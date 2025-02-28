#pragma once

#include "mmath.h"

#include <vector>

class Rasterizer {
public:
    int width;
    int height;

    std::vector<Vector3c> frame_buffer;
    std::vector<uint8_t> stb_frame_buffer;
    std::vector<float> depth_buffer;

    Rasterizer(int width, int height);

    int get_index(int& x, int& y);
    void set_pixel(const Vector3f& p, const Vector3f& col);

    void clear_buffer();
    std::vector<Vector3c> get_frame_buffer();
    std::vector<uint8_t> get_stb_frame_buffer();

    void draw_line(Vector3f& p1, Vector3f& p2, Vector3f& col);
    void draw_triangle(std::vector<Vector3f>& ps, Vector3f& col);
    void draw_triangle_fill(std::vector<Vector3f>& ps, Vector3f& col);

    void draw();
};