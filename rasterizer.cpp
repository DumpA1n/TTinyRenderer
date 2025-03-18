#include "rasterizer.h"

#include <iostream>
#include <algorithm>
#include <cmath> // round floor

Rasterizer::Rasterizer(int width, int height, int channels) {
    this->width = width;
    this->height = height;
    this->channels = channels;

    current_frame_buffer.resize(width * channels * height);

    current_frame_buffer_4x.resize(width * height);
    for (auto& it : current_frame_buffer_4x) { it.resize(4); }

    last_frame_buffer.resize(width * channels * height);

    depth_buffer.resize(width * height);

    depth_buffer_4x.resize(width * height);
    for (auto& it : depth_buffer_4x) { it.resize(4); }
}

int Rasterizer::get_index(const int& x, const int& y) {
    return y * width + x;
}
void Rasterizer::set_pixel(const Vector2i& p, const Vector3f& col) {
    if (p.x < 0 || p.x >= width ||
        p.y < 0 || p.y >= height) return;
    int index = get_index(p.x, p.y) * channels;
    for (int i : {0, 1, 2}) { current_frame_buffer[index + i] = static_cast<uint8_t>(col[i] * 255.0f); }
    if (channels == 4) { current_frame_buffer[index + 3] = 255; }
}
void Rasterizer::set_pixel(int x, int y, const Vector3f& col) {
    set_pixel({x, y}, col);
}

void Rasterizer::clear_buffer(const Vector3f& col) {
    for (int i = 0; i < width * height; i++) {
        int index = i * channels;

        for (int it : {0, 1, 2}) { current_frame_buffer[index + it] = static_cast<uint8_t>(col[it] * 255.0f); }
        if (channels == 4) { current_frame_buffer[index + 3] = 255; }

        std::fill(current_frame_buffer_4x[i].begin(), current_frame_buffer_4x[i].end(), col);

        depth_buffer[i] = FLT_MAX;

        std::fill(depth_buffer_4x[i].begin(), depth_buffer_4x[i].end(), FLT_MAX);
    }
}
std::vector<uint8_t> Rasterizer::get_current_frame_buffer() {
    return current_frame_buffer;
}
std::vector<uint8_t> Rasterizer::get_last_frame_buffer() {
    return last_frame_buffer;
}

void Rasterizer::set_vertex_shader(void* fn) {
    *(void**)(&vertex_shader) = fn;
}
void Rasterizer::set_fragment_shader(void* fn) {
    *(void**)(&fragment_shader) = fn;
}
void Rasterizer::set_texture(Texture* tex) {
    texture = tex;
}
void Rasterizer::add_texture(std::string name, Texture* texture) {
    textureMap.emplace(name, texture);
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector4f* v){
    float c1 = (x*(v[1].y - v[2].y) + (v[2].x - v[1].x)*y + v[1].x*v[2].y - v[2].x*v[1].y) / (v[0].x*(v[1].y - v[2].y) + (v[2].x - v[1].x)*v[0].y + v[1].x*v[2].y - v[2].x*v[1].y);
    float c2 = (x*(v[2].y - v[0].y) + (v[0].x - v[2].x)*y + v[2].x*v[0].y - v[0].x*v[2].y) / (v[1].x*(v[2].y - v[0].y) + (v[0].x - v[2].x)*v[1].y + v[2].x*v[0].y - v[0].x*v[2].y);
    float c3 = (x*(v[0].y - v[1].y) + (v[1].x - v[0].x)*y + v[0].x*v[1].y - v[1].x*v[0].y) / (v[2].x*(v[0].y - v[1].y) + (v[1].x - v[0].x)*v[2].y + v[0].x*v[1].y - v[1].x*v[0].y);
    return {c1,c2,c3};
}
inline static Vector3f interpolate(float alpha, float beta, float gamma, const Vector3f& vert1, const Vector3f& vert2, const Vector3f& vert3, float weight) {
    return (vert1 * alpha + vert2 * beta + vert3 * gamma) / weight;
}
inline static Vector2f interpolate(float alpha, float beta, float gamma, const Vector2f& vert1, const Vector2f& vert2, const Vector2f& vert3, float weight) {
    float u = (alpha * vert1.x + beta * vert2.x + gamma * vert3.x);
    float v = (alpha * vert1.y + beta * vert2.y + gamma * vert3.y);
    u /= weight;
    v /= weight;
    return Vector2f{u, v};
}
inline static Vector3f interpolate(float alpha, float beta, float gamma, const Vector3f* vert, float weight) {
    return (vert[0] * alpha + vert[1] * beta + vert[2] * gamma) / weight;
}
inline static Vector2f interpolate(float alpha, float beta, float gamma, const Vector2f* vert, float weight) {
    float u = (alpha * vert[0].x + beta * vert[1].x + gamma * vert[2].x);
    float v = (alpha * vert[0].y + beta * vert[1].y + gamma * vert[2].y);
    u /= weight;
    v /= weight;
    return Vector2f{u, v};
}
bool Rasterizer::isInsideTriangle2D(const Vector3f& p, Triangle* t) {
    auto v = t->toVector3f().data();
    float n1 = cross(v[1] - v[0], p - v[0]).z;
    float n2 = cross(v[2] - v[1], p - v[1]).z;
    float n3 = cross(v[0] - v[2], p - v[2]).z;
    return (n1 > 0 && n2 > 0 && n3 > 0) || (n1 < 0 && n2 < 0 && n3 < 0);
}
bool Rasterizer::isInsideTriangle2D(float x, float y, Triangle* t) {
    return isInsideTriangle2D({x, y, 1.0f}, t);
}

void Rasterizer::draw_line(const Vector2f& p1, const Vector2f& p2, const Vector3f& col) {
    int x1 = round(p1.x), y1 = round(p1.y);
    int x2 = round(p2.x), y2 = round(p2.y);

    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        set_pixel(x1, y1, col);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx)  { err += dx; y1 += sy; }
    }
}
void Rasterizer::draw_triangle(Triangle* t, const Vector3f& col) {
    auto v = t->toVector2f();
    draw_line(v[0], v[1], col);
    draw_line(v[1], v[2], col);
    draw_line(v[2], v[0], col);
}
void Rasterizer::draw_triangle_filled(Triangle* t, const Vector3f& col) {
    auto v = t->toVector4f();
    Vector2f bottomleft{std::min(std::min(v[0].x, v[1].x), v[2].x), std::min(std::min(v[0].y, v[1].y), v[2].y)};
    Vector2f topright{std::max(std::max(v[0].x, v[1].x), v[2].x), std::max(std::max(v[0].y, v[1].y), v[2].y)};
    for (int x = std::floor(bottomleft.x); x <= std::ceil(topright.x); x++) {
        for (int y = std::floor(bottomleft.y); y <= std::ceil(topright.y); y++) {
            if (isInsideTriangle2D(x, y, t)) {
                set_pixel(x, y, col);
            }
        }
    }
}
void Rasterizer::rasterize(Triangle* t, Vector3f* view_pos) {
    auto v = t->toVector4f();

    // bbox
    Vector2f bottomleft{std::min(std::min(v[0].x, v[1].x), v[2].x), std::min(std::min(v[0].y, v[1].y), v[2].y)};
    Vector2f topright{std::max(std::max(v[0].x, v[1].x), v[2].x), std::max(std::max(v[0].y, v[1].y), v[2].y)};

    for (int x = std::floor(bottomleft.x); x <= std::ceil(topright.x); x++) {
        for (int y = std::floor(bottomleft.y); y <= std::ceil(topright.y); y++) {
            if (x < 0 || x >= width || y < 0 || y >= height) { continue; }

            static float AA4xSteps[4][2] = {
                {0.25, 0.25},
                {0.75, 0.25},
                {0.25, 0.75},
                {0.75, 0.75}
            };

            if (MSAA4x) {
                bool need_sampler = false;
                bool inside_sample[4] = {false};
                for (int i = 0; i < 4; i++) {
                    auto [sx, sy] = [&]() -> std::pair<float, float> { return {x + AA4xSteps[i][0], y + AA4xSteps[i][1]}; }();
                    if (isInsideTriangle2D(sx, sy, t)) {
                        auto[alpha, beta, gamma] = computeBarycentric2D(sx, sy, v.data());
                        float Z = 1.0 / (alpha / v[0].w + beta / v[1].w + gamma / v[2].w);
                        float zp = alpha * v[0].z / v[0].w + beta * v[1].z / v[1].w + gamma * v[2].z / v[2].w;
                        zp *= Z;
                        float& depth = depth_buffer_4x[y * width + x][i];
                        if (zp < depth) {
                            need_sampler = true;
                            inside_sample[i] = true;
                            depth = zp;
                        }
                    }
                    if (need_sampler) {
                        auto[alpha, beta, gamma] = computeBarycentric2D(x, y, v.data());
                        Vector3f interpolated_color = interpolate(alpha, beta, gamma, t->color, 1);
                        Vector3f interpolated_normal = interpolate(alpha, beta, gamma, t->normals, 1);
                        Vector2f interpolated_texcoords = interpolate(alpha, beta, gamma, t->tex_coords, 1);
                        Vector3f interpolated_shadingcoords = interpolate(alpha, beta, gamma, view_pos, 1);

                        fragment_shader_payload payload({interpolated_color, interpolated_normal, interpolated_texcoords, texture, &textureMap});
                        payload.view_pos = interpolated_shadingcoords;

                        Vector3f gl_FragColor = fragment_shader(payload);

                        for (int samp = 0; samp < 4; samp++) {
                            if (inside_sample[samp]) {
                                current_frame_buffer_4x[y * width + x][samp] = gl_FragColor;
                            }
                        }
                    }
                }
            } else {
                if (isInsideTriangle2D(x, y, t)) {

                    // calc depth
                    auto[alpha, beta, gamma] = computeBarycentric2D(x, y, v.data());
                    float Z = 1.0 / (alpha / v[0].w + beta / v[1].w + gamma / v[2].w);
                    float zp = alpha * v[0].z / v[0].w + beta * v[1].z / v[1].w + gamma * v[2].z / v[2].w;
                    zp *= Z;

                    // depth test
                    if (zp < depth_buffer[y * width + x]) {
                        // Triangle Barycentric Interpolation
                        Vector3f interpolated_color = interpolate(alpha, beta, gamma, t->color, 1);
                        Vector3f interpolated_normal = interpolate(alpha, beta, gamma, t->normals, 1);
                        Vector2f interpolated_texcoords = interpolate(alpha, beta, gamma, t->tex_coords, 1);
                        Vector3f interpolated_shadingcoords = interpolate(alpha, beta, gamma, view_pos, 1);

                        fragment_shader_payload payload({interpolated_color, interpolated_normal, interpolated_texcoords, texture, &textureMap});
                        payload.view_pos = interpolated_shadingcoords;

                        set_pixel(x, y, fragment_shader(payload));

                        depth_buffer[y * width + x] = zp;
                    }
                }
            }
        }
    }
}
void Rasterizer::ViewPort(Vector4f& p, int w, int h) {
    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;
    p.x = w*0.5f*(p.x+1.0f);
    p.y = h*(1.0f - 0.5f*(p.y+1.0f));
    p.z = p.z * f1 + f2;
};

void Rasterizer::draw(std::vector<Triangle*> triangles) {
    angleY = ((int)angleY + 2) % 360;
    for (const auto& t : triangles) {
        Triangle newTri = *t;
        Vector3f view_pos[3];

        for (int i = 0; i < 3; i++)
            vertex_shader({newTri.vertices[i], newTri.normals[i], view_pos[i]});

        for (int i = 0; i < 3; i++)
            ViewPort(newTri.vertices[i], width, height);

        if (renderMode == TEXTURE_MODE)
            rasterize(&newTri, view_pos);
        else if (renderMode == LINE_FRAME_MODE)
            draw_triangle(&newTri, Vector3f{1.0f});
        else if (renderMode == PURE_COLOR_MODE)
            draw_triangle_filled(&newTri, Vector3f{1.0f});
    }

    if (MSAA4x) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                Vector3f accumulate_color;
                for (int i = 0; i < 4; i++) {
                    accumulate_color += current_frame_buffer_4x[y * width + x][i];
                }
                set_pixel(x, y, accumulate_color / 4.0f);
            }
        }
    }
}

void Rasterizer::setRenderMode(enum RENDER_MODE mode) {
    this->renderMode = mode;
}
enum RENDER_MODE Rasterizer::getRenderMode() {
    return this->renderMode;
}
void Rasterizer::setMSAA4x(bool state) {
    this->MSAA4x = state;
}
bool Rasterizer::getMSAA4x() {
    return this->MSAA4x;
}
void Rasterizer::setSSAA4x(bool state) {
    this->SSAA4x = state;
}
bool Rasterizer::getSSAA4x() {
    return this->SSAA4x;
}
