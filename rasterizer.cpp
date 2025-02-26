#include "rasterizer.h"

Rasterizer::Rasterizer(int width, int height) {
    this->width = width;
    this->height = height;
    frame_buffer.resize(width * height);
    depth_buffer.resize(width * height);
}

int Rasterizer::get_index(int& x, int& y) {
    return (height - y) * width + x;
}
void Rasterizer::set_pixel(Point& p, Vector3f& col) {
    if (p.x < 0 || p.x >= width ||
        p.y < 0 || p.y >= height) return;
    int index = (height - p.y) * width + p.x;
    frame_buffer[index] = Vector3c{(char)(col.x * 255.0f), (char)(col.y * 255.0f), (char)(col.z * 255.0f)};
}

void Rasterizer::clear_buffer() {
    for (int i = 0; i < width * height; i++) {
        frame_buffer[i] = Vector3c{0, 0, 0};
        depth_buffer[i] = 0xFFFF7F7F;
    }
}
std::vector<Vector3c> Rasterizer::get_frame_buffer() {
    return frame_buffer;
}

void Rasterizer::draw_line(Point& p1, Point& p2, Vector3f& col) {
    
}
void Rasterizer::draw_triangle(std::vector<Point>& ps, Vector3f& col) {
    draw_line(ps[0], ps[1], col);
    draw_line(ps[1], ps[2], col);
    draw_line(ps[2], ps[0], col);
}