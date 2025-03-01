#include "rasterizer.h"

#include <iostream>
#include <algorithm>
#include <cmath> // round floor

Rasterizer::Rasterizer(int width, int height) {
    this->width = width;
    this->height = height;
    frame_buffer.resize(width * height);
    depth_buffer.resize(width * height);
    stb_frame_buffer.resize(width * height * 3);
}

int Rasterizer::get_index(int& x, int& y) {
    // return (height - y) * width + x;
    return y * width + x;
}
void Rasterizer::set_pixel(const Vector3f& p, const Vector3f& col) {
    if (p.x < 0 || p.x >= width ||
        p.y < 0 || p.y >= height) return;
    // int index = (height - p.y) * width + p.x;
    int index = p.y * width + p.x;
    frame_buffer[index] = Vector3c{(int8_t)(col.x * 255.0f), (int8_t)(col.y * 255.0f), (int8_t)(col.z * 255.0f)};
    stb_frame_buffer[index * 3 + 0] = frame_buffer[index].x;
    stb_frame_buffer[index * 3 + 1] = frame_buffer[index].y;
    stb_frame_buffer[index * 3 + 2] = frame_buffer[index].z;
}

void Rasterizer::clear_buffer() {
    for (int i = 0; i < width * height; i++) {
        frame_buffer[i] = Vector3c{0, 0, 0};
        depth_buffer[i] = 0xFFFF7F7F;
        stb_frame_buffer[i * 3 + 0] = 0;
        stb_frame_buffer[i * 3 + 1] = 0;
        stb_frame_buffer[i * 3 + 2] = 0;
    }
}
std::vector<Vector3c> Rasterizer::get_frame_buffer() {
    return frame_buffer;
}
std::vector<uint8_t> Rasterizer::get_stb_frame_buffer() {
    return stb_frame_buffer;
}

void Rasterizer::set_vertex_shader(void* fn) {
    *(void**)(&vertex_shader) = fn;
}
void Rasterizer::set_fragment_shader(void* fn) {
    *(void**)(&fragment_shader) = fn;
}

void Rasterizer::draw_line(const Vector3f& p1, const Vector3f& p2, const Vector3f& col) {
    int x1 = round(p1.x), y1 = round(p1.y);
    int x2 = round(p2.x), y2 = round(p2.y);

    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        set_pixel({(float)x1, (float)y1}, col);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx)  { err += dx; y1 += sy; }
    }
}
void Rasterizer::draw_triangle(const std::vector<Vector3f>& ps, const Vector3f& col) {
    draw_line(ps[0], ps[1], col);
    draw_line(ps[1], ps[2], col);
    draw_line(ps[2], ps[0], col);
}
bool inside_triangle(const Vector3f& p, const Vector3f* tri) {
    float n1 = (tri[1] - tri[0]).cross(p - tri[0]).z;
    float n2 = (tri[2] - tri[1]).cross(p - tri[1]).z;
    float n3 = (tri[0] - tri[2]).cross(p - tri[2]).z;
    return (n1 > 0 && n2 > 0 && n3 > 0) || (n1 < 0 && n2 < 0 && n3 < 0);
}
void Rasterizer::draw_triangle_fill(const std::vector<Vector3f>& ps, const Vector3f& col) {
    Vector3f bottomleft{std::min(std::min(ps[0].x, ps[1].x), ps[2].x), std::min(std::min(ps[0].y, ps[1].y), ps[2].y)};
    Vector3f topright{std::max(std::max(ps[0].x, ps[1].x), ps[2].x), std::max(std::max(ps[0].y, ps[1].y), ps[2].y)};

    for (int x = std::floor(bottomleft.x); x <= std::ceil(topright.x); x++) {
        for (int y = std::floor(bottomleft.y); y <= std::ceil(topright.y); y++) {
            Vector3f thizp{(float)x, (float)y};
            if (inside_triangle(thizp, ps.data())) {
                set_pixel(thizp, col);
            }
        }
    }
}
void Rasterizer::draw_triangle_filled(Triangle* tri) {
    Vector3f* p = tri->vertices;
    Vector3f bottomleft{std::min(std::min(p[0].x, p[1].x), p[2].x), std::min(std::min(p[0].y, p[1].y), p[2].y)};
    Vector3f topright{std::max(std::max(p[0].x, p[1].x), p[2].x), std::max(std::max(p[0].y, p[1].y), p[2].y)};
    for (int x = std::floor(bottomleft.x); x <= std::ceil(topright.x); x++) {
        for (int y = std::floor(bottomleft.y); y <= std::ceil(topright.y); y++) {
            Vector3f thizp{(float)x, (float)y};
            if (inside_triangle(thizp, p)) {
                set_pixel(thizp, tri->color[0]);
            }
        }
    }
}


void Rasterizer::draw(std::vector<Triangle*> triangels) {
    
}