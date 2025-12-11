#pragma once

#include "mmath.h"
#include "shader/ishader.h"
#include "triangle.h"

#include <vector>
#include <string>
#include <unordered_map>

enum RENDER_MODE {
    TEXTURE_MODE = 1,
    LINE_FRAME_MODE = 2,
    PURE_COLOR_MODE = 3
};

class Rasterizer {
public:
    int width;
    int height;
    int channels;

    Rasterizer(int width, int height, int stride);

    void initialize(int width, int height, int stride);

    RENDER_MODE renderMode = TEXTURE_MODE;
    bool MSAA4x = true;
    bool SSAA4x = false;

    std::vector<uint8_t> current_frame_buffer;
    std::vector<std::vector<Vector3f>> current_frame_buffer_4x;
    std::vector<uint8_t> last_frame_buffer;
    std::vector<float> depth_buffer;
    std::vector<std::vector<float>> depth_buffer_4x;

    inline int get_index(const int& x, const int& y);
    inline void set_pixel(const Vector2i& p, const Vector3f& col);
    inline void set_pixel(int x, int y, const Vector3f& col);
    void clear_buffer(const Vector3f& col);
    std::vector<uint8_t> get_current_frame_buffer();
    std::vector<uint8_t> get_last_frame_buffer();

    IShader* shader_ = nullptr;
    void set_shader(IShader* shader);

    Texture* texture = nullptr;
    void set_texture(Texture* tex);
    std::unordered_map<std::string, Texture*> textureMap;
    void add_texture(std::string name, Texture* texture);

    bool isInsideTriangle2D(const Vector3f& p, const Triangle& t);
    bool isInsideTriangle2D(float x, float y, const Triangle& t);

    void draw_line(const Vector2f& p1, const Vector2f& p2, const Vector3f& col);
    void draw_triangle(const Triangle& t, const Vector3f& col);
    void draw_triangle_filled(const Triangle& t, const Vector3f& col);
    void rasterize(const Triangle& t, const std::array<Vector3f, 3>& view_pos);
    void ViewPort(Vector4f& p, int w, int h);

    void draw(const std::vector<Triangle>& triangles);
    void draw_multi_thread(const std::vector<Triangle>& triangles);

    void setRenderMode(enum RENDER_MODE mode);
    enum RENDER_MODE getRenderMode();
    void setMSAA4x(bool state);
    bool getMSAA4x();
    void setSSAA4x(bool state);
    bool getSSAA4x();

};