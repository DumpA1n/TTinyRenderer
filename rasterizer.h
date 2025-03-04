#pragma once

#include "mmath.h"
#include "shader.h"
#include "triangle.h"

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

    Vector3f (*vertex_shader)(const vertex_shader_payload& payload);
    Vector3f (*fragment_shader)(const fragment_shader_payload& payload);
    void set_vertex_shader(void* fn);
    void set_fragment_shader(void* fn);

    Texture* texture;
    void set_texture(Texture* tex);

    void draw_line(const Vector3f& p1, const Vector3f& p2, const Vector3f& col);
    void draw_triangle(const std::vector<Vector3f>& ps, const Vector3f& col);
    void draw_triangle_fill(const std::vector<Vector3f>& ps, const Vector3f& col);
    void draw_triangle_filled(Triangle* tri);

    void draw(std::vector<Triangle*> triangels);
};