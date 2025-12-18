#include "rasterizer.h"

#include <array>
#include <algorithm>
#include <cmath> // round floor
#include <cfloat> // FLT_MAX
#include <future> // future
#include <memory>

#include "object/object.h"
#include "scene/scene.h"
#include "shader/ishader.h"
#include "utils/ThreadPool.h"
#include "utils/mmath.h"

Rasterizer::Rasterizer(int width, int height, int channels)
    : width_(width), height_(height), channels_(channels)
{
}

void Rasterizer::initialize() {
    current_frame_buffer_.resize(width_ * channels_ * height_);

    current_frame_buffer_4x_.resize(width_ * height_);
    for (auto& it : current_frame_buffer_4x_) { it.resize(4); }

    last_frame_buffer_.resize(width_ * channels_ * height_);

    depth_buffer_.resize(width_ * height_);

    depth_buffer_4x_.resize(width_ * height_);
    for (auto& it : depth_buffer_4x_) { it.resize(4); }
}

int Rasterizer::get_index(int x, int y) const {
    return y * width_ + x;
}

void Rasterizer::set_pixel(const Vector2i& p, const Vector3f& col) {
    if (p.x < 0 || p.x >= width_ ||
        p.y < 0 || p.y >= height_) return;
    int index = get_index(p.x, p.y) * channels_;
    for (int i : {0, 1, 2}) { current_frame_buffer_[index + i] = static_cast<uint8_t>(col[i] * 255.0f); }
    if (channels_ == 4) { current_frame_buffer_[index + 3] = 255; }
}

void Rasterizer::set_pixel(int x, int y, const Vector3f& col) {
    set_pixel({x, y}, col);
}

void Rasterizer::clear_buffer(const Vector3f& col) {
    for (int i = 0; i < width_ * height_; i++) {
        int index = i * channels_;

        for (int it : {0, 1, 2}) { current_frame_buffer_[index + it] = static_cast<uint8_t>(col[it] * 255.0f); }
        if (channels_ == 4) { current_frame_buffer_[index + 3] = 255; }

        std::fill(current_frame_buffer_4x_[i].begin(), current_frame_buffer_4x_[i].end(), col);

        depth_buffer_[i] = FLT_MAX;

        std::fill(depth_buffer_4x_[i].begin(), depth_buffer_4x_[i].end(), FLT_MAX);
    }
}

void Rasterizer::set_model_matrix(const Vector3f& translate, const Vector3f& rotate, const Vector3f& scale) {
    float rx = degrees_to_radians(rotate.x);
    float ry = degrees_to_radians(rotate.y);
    float rz = degrees_to_radians(rotate.z);
    float cx = cos(rx), sx = sin(rx);
    float cy = cos(ry), sy = sin(ry);
    float cz = cos(rz), sz = sin(rz);

    model_matrix_ = Matrix4f{
        scale.x * (cy * cz), scale.x * (cz * sx * sy - cx * sz), scale.x * (cx * cz * sy + sx * sz), translate.x,
        scale.y * (cy * sz), scale.y * (cx * cz + sx * sy * sz), scale.y * (cx * sy * sz - cz * sx), translate.y,
        scale.z * (-sy)    , scale.z * (cy * sx)               , scale.z * (cx * cy)               , translate.z,
        0.0f               , 0.0f                              , 0.0f                              , 1.0f
    };
}

void Rasterizer::set_view_matrix(const Vector3f& eye_pos) {
    Vector3f up{0, 1, 0};
    Vector3f target{0, 0, 0};
    Vector3f z_axis = normalized(eye_pos - target);
    Vector3f x_axis = normalized(cross(up, z_axis));
    Vector3f y_axis = cross(z_axis, x_axis);

    view_matrix_ = Matrix4f{
        x_axis.x, x_axis.y, x_axis.z, -(x_axis * eye_pos),
        y_axis.x, y_axis.y, y_axis.z, -(y_axis * eye_pos),
        z_axis.x, z_axis.y, z_axis.z, -(z_axis * eye_pos),
        0,        0,        0,        1
    };
}

void Rasterizer::set_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar) {
    eye_fov = degrees_to_radians(eye_fov);

    projection_matrix_ = Matrix4f{
        1/(tan(eye_fov/2)*aspect_ratio), 0               , 0                         , 0,
        0                              , 1/tan(eye_fov/2), 0                         , 0,
        0                              , 0               , (zNear+zFar)/(zNear-zFar), (2*zNear*zFar)/(zNear-zFar),
        0                              , 0               , -1                        , 0, 
    };
}

void Rasterizer::ViewPort(Vector4f& p, int w, int h) {
    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;
    p.x = w*0.5f*(p.x+1.0f);
    p.y = h*(1.0f - 0.5f*(p.y+1.0f));
    p.z = p.z * f1 + f2;
}

std::tuple<float, float, float> Rasterizer::ComputeBarycentric2D(float x, float y, const std::array<Vector4f, 3>& v) {
    float c1 = (x*(v[1].y - v[2].y) + (v[2].x - v[1].x)*y + v[1].x*v[2].y - v[2].x*v[1].y) / (v[0].x*(v[1].y - v[2].y) + (v[2].x - v[1].x)*v[0].y + v[1].x*v[2].y - v[2].x*v[1].y);
    float c2 = (x*(v[2].y - v[0].y) + (v[0].x - v[2].x)*y + v[2].x*v[0].y - v[0].x*v[2].y) / (v[1].x*(v[2].y - v[0].y) + (v[0].x - v[2].x)*v[1].y + v[2].x*v[0].y - v[0].x*v[2].y);
    float c3 = (x*(v[0].y - v[1].y) + (v[1].x - v[0].x)*y + v[0].x*v[1].y - v[1].x*v[0].y) / (v[2].x*(v[0].y - v[1].y) + (v[1].x - v[0].x)*v[2].y + v[0].x*v[1].y - v[1].x*v[0].y);
    return {c1,c2,c3};
}

Vector3f Rasterizer::Interpolate(float alpha, float beta, float gamma, const Vector3f& vert1, const Vector3f& vert2, const Vector3f& vert3, float weight) {
    return (vert1 * alpha + vert2 * beta + vert3 * gamma) / weight;
}

Vector3f Rasterizer::Interpolate(float alpha, float beta, float gamma, const std::array<Vector3f, 3>& vert, float weight) {
    return (vert[0] * alpha + vert[1] * beta + vert[2] * gamma) / weight;
}

Vector2f Rasterizer::Interpolate(float alpha, float beta, float gamma, const Vector2f& vert1, const Vector2f& vert2, const Vector2f& vert3, float weight) {
    float u = (alpha * vert1.x + beta * vert2.x + gamma * vert3.x);
    float v = (alpha * vert1.y + beta * vert2.y + gamma * vert3.y);
    u /= weight;
    v /= weight;
    return Vector2f{u, v};
}

Vector2f Rasterizer::Interpolate(float alpha, float beta, float gamma, const std::array<Vector2f, 3>& vert, float weight) {
    float u = (alpha * vert[0].x + beta * vert[1].x + gamma * vert[2].x);
    float v = (alpha * vert[0].y + beta * vert[1].y + gamma * vert[2].y);
    u /= weight;
    v /= weight;
    return Vector2f{u, v};
}

bool Rasterizer::IsInsideTriangle2D(const Vector3f& p, const Triangle& t) {
    auto v = t.toVector3f();
    float n1 = cross(v[1] - v[0], p - v[0]).z;
    float n2 = cross(v[2] - v[1], p - v[1]).z;
    float n3 = cross(v[0] - v[2], p - v[2]).z;
    return (n1 > 0 && n2 > 0 && n3 > 0) || (n1 < 0 && n2 < 0 && n3 < 0);
}

bool Rasterizer::IsInsideTriangle2D(float x, float y, const Triangle& t) {
    return IsInsideTriangle2D({x, y, 1.0f}, t);
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

void Rasterizer::draw_triangle(const Triangle& t, const Vector3f& col) {
    auto v = t.toVector2f();
    draw_line(v[0], v[1], col);
    draw_line(v[1], v[2], col);
    draw_line(v[2], v[0], col);
}

void Rasterizer::draw_triangle_filled(const Triangle& t, const Vector3f& col) {
    auto v = t.toVector4f();
    Vector2f bottomleft{std::min(std::min(v[0].x, v[1].x), v[2].x), std::min(std::min(v[0].y, v[1].y), v[2].y)};
    Vector2f topright{std::max(std::max(v[0].x, v[1].x), v[2].x), std::max(std::max(v[0].y, v[1].y), v[2].y)};
    for (int x = std::floor(bottomleft.x); x <= std::ceil(topright.x); x++) {
        for (int y = std::floor(bottomleft.y); y <= std::ceil(topright.y); y++) {
            if (IsInsideTriangle2D(x, y, t)) {
                set_pixel(x, y, col);
            }
        }
    }
}

void Rasterizer::rasterize(const std::shared_ptr<Object> obj, const Triangle& t, const std::array<Vector3f, 3>& view_pos) {
    auto v = t.toVector4f();

    // bbox
    Vector2f bottomleft{std::min(std::min(v[0].x, v[1].x), v[2].x), std::min(std::min(v[0].y, v[1].y), v[2].y)};
    Vector2f topright{std::max(std::max(v[0].x, v[1].x), v[2].x), std::max(std::max(v[0].y, v[1].y), v[2].y)};

    for (int x = std::floor(bottomleft.x); x <= std::ceil(topright.x); x++) {
        for (int y = std::floor(bottomleft.y); y <= std::ceil(topright.y); y++) {
            if (x < 0 || x >= width_ || y < 0 || y >= height_) { continue; }

            if (IsInsideTriangle2D(x, y, t)) {

                // calc depth
                auto[alpha, beta, gamma] = ComputeBarycentric2D(x, y, v);
                float Z = 1.0 / (alpha / v[0].w + beta / v[1].w + gamma / v[2].w);
                float zp = alpha * v[0].z / v[0].w + beta * v[1].z / v[1].w + gamma * v[2].z / v[2].w;
                zp *= Z;

                // depth test
                if (zp < depth_buffer_[y * width_ + x]) {
                    // Triangle Barycentric Interpolation
                    Vector3f interpolated_color = Interpolate(alpha, beta, gamma, t.color, 1);
                    Vector3f interpolated_normal = Interpolate(alpha, beta, gamma, t.normals, 1);
                    Vector2f interpolated_texcoords = Interpolate(alpha, beta, gamma, t.tex_coords, 1);
                    Vector3f interpolated_shadingcoords = Interpolate(alpha, beta, gamma, view_pos, 1);

                    fragment_shader_payload payload{
                        .object = obj,
                        .scene = scene(),
                        .view_pos = interpolated_shadingcoords,
                        .normal = interpolated_normal,
                        .tex_coord = interpolated_texcoords,
                        .color = interpolated_color
                    };

                    if (auto shader = obj->shader(); shader != nullptr) {
                        set_pixel(x, y, shader->fragment_shader(payload).color);
                    }

                    depth_buffer_[y * width_ + x] = zp;
                }
            }
        }
    }
}

void Rasterizer::run() {
    Matrix4f view_mat = (set_view_matrix(scene()->camera()->position()), get_view_matrix());
    Matrix4f proj_mat = (set_projection_matrix(45.0f, 1.0f, 0.1f, 50.0f), get_projection_matrix());

    for (const auto& obj : scene()->objects()) {
        Matrix4f model_mat = (set_model_matrix(obj->position(), obj->rotation(), obj->scale()), get_model_matrix());

        for (const auto& t : obj->triangles()) {
            auto task = [&]() {
                Triangle vs_tri = t;
                std::array<Vector3f, 3> view_pos;

                for (int i = 0; i < 3; i++) {
                    vertex_shader_payload vs_input{
                        .object = obj,
                        .position = vs_tri.vertices[i],
                        .normal = vs_tri.normals[i],
                        .model = model_mat,
                        .view = view_mat,
                        .projection = proj_mat
                    };

                    if (auto shader = obj->shader(); shader != nullptr) {
                        auto vs_output = shader->vertex_shader(vs_input);

                        vs_tri.vertices[i] = vs_output.position;
                        vs_tri.normals[i] = vs_output.normal;
                        view_pos[i] = vs_output.view_pos;
                    }
                }

                for (int i = 0; i < 3; i++)
                    ViewPort(vs_tri.vertices[i], width_, height_);

                if (render_params_.render_mode == TEXTURE_MODE)
                    rasterize(obj, vs_tri, view_pos);
            };

            task();
        }
    }
}

void Rasterizer::run_multi_thread() {
    static ThreadPool thread_pool(std::thread::hardware_concurrency());
    std::vector<std::future<void>> tasks;

    Matrix4f view_mat = (set_view_matrix(scene()->camera()->position()), get_view_matrix());
    Matrix4f proj_mat = (set_projection_matrix(45.0f, 1.0f, 0.1f, 50.0f), get_projection_matrix());

    for (const auto& obj : scene()->objects()) {
        Matrix4f model_mat = (set_model_matrix(obj->position(), obj->rotation(), obj->scale()), get_model_matrix());

        const auto& triangles = obj->triangles();
        const size_t total = triangles.size();
        const size_t batch_size = std::max(size_t(1), total / (std::thread::hardware_concurrency() * 2));

        for (size_t start = 0; start < total; start += batch_size) {
            size_t end = std::min(start + batch_size, total);

            tasks.push_back(thread_pool.enqueue([this, &obj, &triangles, start, end, model_mat, view_mat, proj_mat]() {
                for (size_t idx = start; idx < end; idx++) {
                    Triangle vs_tri = triangles[idx];
                    std::array<Vector3f, 3> view_pos;

                    for (int i = 0; i < 3; i++) {
                        vertex_shader_payload vs_input{
                            .object = obj,
                            .position = vs_tri.vertices[i],
                            .normal = vs_tri.normals[i],
                            .model = model_mat,
                            .view = view_mat,
                            .projection = proj_mat
                        };

                        if (auto shader = obj->shader(); shader != nullptr) {
                            auto vs_output = shader->vertex_shader(vs_input);

                            vs_tri.vertices[i] = vs_output.position;
                            vs_tri.normals[i] = vs_output.normal;
                            view_pos[i] = vs_output.view_pos;
                        }
                    }

                    for (int i = 0; i < 3; i++)
                        ViewPort(vs_tri.vertices[i], width_, height_);

                    if (render_params_.render_mode == TEXTURE_MODE)
                        rasterize(obj, vs_tri, view_pos);
                    else if (render_params_.render_mode == LINE_FRAME_MODE)
                        draw_triangle(vs_tri, Vector3f{1.0f});
                    else if (render_params_.render_mode == PURE_COLOR_MODE)
                        draw_triangle_filled(vs_tri, Vector3f{1.0f});
                }
            }));
        }
    }

    for (auto& t : tasks) t.get();
}
