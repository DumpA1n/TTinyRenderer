#pragma once

#include "mmath.h"
#include "shader.h"
#include "triangle.h"

#include <vector>
#include <string>
#include <unordered_map>

enum DRAW_MODE {
    RASTERIZE_MODE = 1,
    LINE_FRAME_MODE = 2,
    PURE_COLOR_MODE = 3
};

class Rasterizer {
public:
    int width;
    int height;
    int channels;

    bool SSAA4x = false;
    bool MSAA4x = true;

    std::vector<uint8_t> current_frame_buffer;
    std::vector<std::vector<Vector3f>> current_frame_buffer_4x;
    std::vector<uint8_t> last_frame_buffer;
    std::vector<float> depth_buffer;
    std::vector<std::vector<float>> depth_buffer_4x;

    Rasterizer(int width, int height, int stride);

    inline int get_index(const int& x, const int& y);
    inline void set_pixel(const Vector2i& p, const Vector3f& col);
    inline void set_pixel(int x, int y, const Vector3f& col);
    void clear_buffer(const Vector3f& col);
    std::vector<uint8_t> get_current_frame_buffer();
    std::vector<uint8_t> get_last_frame_buffer();

    Vector4f (*vertex_shader)(const vertex_shader_payload& payload);
    Vector3f (*fragment_shader)(const fragment_shader_payload& payload);
    void set_vertex_shader(void* fn);
    void set_fragment_shader(void* fn);

    Texture* texture = nullptr;
    void set_texture(Texture* tex);
    std::unordered_map<std::string, Texture*> textureMap;
    void add_texture(std::string name, Texture* texture);

    inline bool isInsideTriangle2D(const Vector3f& p, Triangle* t);
    inline bool isInsideTriangle2D(float x, float y, Triangle* t);

    void draw_line(const Vector2f& p1, const Vector2f& p2, const Vector3f& col);
    void draw_triangle(Triangle* t, const Vector3f& col);
    void draw_triangle_filled(Triangle* t, const Vector3f& col);
    void rasterize(Triangle* t, Vector3f* view_pos);
    void ViewPort(Vector4f& p, int w, int h);

    void draw(std::vector<Triangle*> triangles, enum DRAW_MODE mode);
};