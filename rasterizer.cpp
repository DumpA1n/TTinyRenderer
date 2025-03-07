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
void Rasterizer::set_pixel(const Vector2i& p, const Vector3f& col) {
    if (p.x < 0 || p.x >= width ||
        p.y < 0 || p.y >= height) return;
    // int index = (height - p.y) * width + p.x;
    int index = p.y * width + p.x;
    frame_buffer[index] = Vector3c{(uint8_t)(col.x * 255.0f), (uint8_t)(col.y * 255.0f), (uint8_t)(col.z * 255.0f)};
    stb_frame_buffer[index * 3 + 0] = frame_buffer[index].x;
    stb_frame_buffer[index * 3 + 1] = frame_buffer[index].y;
    stb_frame_buffer[index * 3 + 2] = frame_buffer[index].z;
}
void Rasterizer::set_pixel(int x, int y, const Vector3f& col) {
    if (x < 0 || x >= width ||
        y < 0 || y >= height) return;
    // int index = (height - p.y) * width + p.x;
    int index = y * width + x;
    frame_buffer[index] = Vector3c{(uint8_t)(col.x * 255.0f), (uint8_t)(col.y * 255.0f), (uint8_t)(col.z * 255.0f)};
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
void Rasterizer::set_texture(Texture* tex) {
    texture = tex;
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector4f* v){
    float c1 = (x*(v[1].y - v[2].y) + (v[2].x - v[1].x)*y + v[1].x*v[2].y - v[2].x*v[1].y) / (v[0].x*(v[1].y - v[2].y) + (v[2].x - v[1].x)*v[0].y + v[1].x*v[2].y - v[2].x*v[1].y);
    float c2 = (x*(v[2].y - v[0].y) + (v[0].x - v[2].x)*y + v[2].x*v[0].y - v[0].x*v[2].y) / (v[1].x*(v[2].y - v[0].y) + (v[0].x - v[2].x)*v[1].y + v[2].x*v[0].y - v[0].x*v[2].y);
    float c3 = (x*(v[0].y - v[1].y) + (v[1].x - v[0].x)*y + v[0].x*v[1].y - v[1].x*v[0].y) / (v[2].x*(v[0].y - v[1].y) + (v[1].x - v[0].x)*v[2].y + v[0].x*v[1].y - v[1].x*v[0].y);
    return {c1,c2,c3};
}
static Vector3f interpolate(float alpha, float beta, float gamma, const Vector3f& vert1, const Vector3f& vert2, const Vector3f& vert3, float weight) {
    return (vert1 * alpha + vert2 * beta + vert3 * gamma) / weight;
}
static Vector2f interpolate(float alpha, float beta, float gamma, const Vector2f& vert1, const Vector2f& vert2, const Vector2f& vert3, float weight) {
    float u = (alpha * vert1.x + beta * vert2.x + gamma * vert3.x);
    float v = (alpha * vert1.y + beta * vert2.y + gamma * vert3.y);
    u /= weight;
    v /= weight;
    return Vector2f{u, v};
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
void Rasterizer::draw_triangle(const std::vector<Vector2f>& ps, const Vector3f& col) {
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
            if (inside_triangle({x, y}, ps.data())) {
                set_pixel(x, y, col);
            }
        }
    }
}
void Rasterizer::draw_triangle_filled(Triangle* t, Vector3f* view_pos) {
    auto v = t->toVector4f();

    //bbox
    Vector3f bottomleft{std::min(std::min(v[0].x, v[1].x), v[2].x), std::min(std::min(v[0].y, v[1].y), v[2].y)};
    Vector3f topright{std::max(std::max(v[0].x, v[1].x), v[2].x), std::max(std::max(v[0].y, v[1].y), v[2].y)};

    for (int x = std::floor(bottomleft.x); x <= std::ceil(topright.x); x++) {
        for (int y = std::floor(bottomleft.y); y <= std::ceil(topright.y); y++) {
            if (x < 0 || x >= width || y < 0 || y >= height) { continue; }

            if (inside_triangle({x, y}, t->toVector3f().data())) {

                // calc depth
                auto[alpha, beta, gamma] = computeBarycentric2D(x, y, v.data());
                float Z = 1.0 / (alpha / v[0].w + beta / v[1].w + gamma / v[2].w);
                float zp = alpha * v[0].z / v[0].w + beta * v[1].z / v[1].w + gamma * v[2].z / v[2].w;
                zp *= Z;

                // depth test
                bool b_Depth_Testing = true;
                if (b_Depth_Testing && zp < depth_buffer[y * width + x]) {

                    // Triangle Barycentric Interpolation
                    bool b_Triangle_Barycentric_Interpolation = true;
                    if (b_Triangle_Barycentric_Interpolation) {
                        Vector3f interpolated_color = interpolate(alpha, beta, gamma, t->color[0], t->color[1], t->color[2], 1);
                        Vector3f interpolated_normal = interpolate(alpha, beta, gamma, t->normals[0], t->normals[1], t->normals[2], 1);
                        Vector2f interpolated_texcoords = interpolate(alpha, beta, gamma, t->tex_coords[0], t->tex_coords[1], t->tex_coords[2], 1);
                        Vector3f interpolated_shadingcoords = interpolate(alpha, beta, gamma, view_pos[0], view_pos[1], view_pos[2], 1);
                        fragment_shader_payload payload({interpolated_color, interpolated_normal, interpolated_texcoords, texture});
                        payload.view_pos = interpolated_shadingcoords;
                        set_pixel(x, y, fragment_shader(payload));
                    } else {
                        set_pixel(x, y, fragment_shader({t->color[0], (t->normals[0]+t->normals[1]+t->normals[2])/3.0f, t->tex_coords[0], texture}));
                    }
                    depth_buffer[y * width + x] = zp;
                } else {

                }
            }
        }
    }
}

void Rasterizer::draw(std::vector<Triangle*> triangels) {
    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    constexpr float PI = 3.14159265358979323846f;
    static auto degrees_to_radians = [&](float degree) {
        return degree * (PI / 180.0f);
    };

    static auto get_model_matrix = [&](const Vector3f& scale, const Vector3f& rotate, const Vector3f& translate) {
        float rx = degrees_to_radians(rotate.x);
        float ry = degrees_to_radians(rotate.y);
        float rz = degrees_to_radians(rotate.z);
        float cx = cos(rx), sx = sin(rx);
        float cy = cos(ry), sy = sin(ry);
        float cz = cos(rz), sz = sin(rz);
        return Matrix4f{
            scale.x * (cy * cz), scale.x * (cz * sx * sy - cx * sz), scale.x * (cx * cz * sy + sx * sz), translate.x,
            scale.y * (cy * sz), scale.y * (cx * cz + sx * sy * sz), scale.y * (cx * sy * sz - cz * sx), translate.y,
            scale.z * (-sy)    , scale.z * (cy * sx)               , scale.z * (cx * cy)               , translate.z,
            0.0f               , 0.0f                              , 0.0f                              , 1.0f
        };
    };

    static auto get_view_matrix = [&](const Vector3f& eye_pos) {
        Matrix4f view = Matrix4f::identity();
        Matrix4f translate{
            1, 0, 0, -eye_pos.x,
            0, 1, 0, -eye_pos.y,
            0, 0, 1, -eye_pos.z,
            0, 0, 0, 1
        };
        return translate * view;
    };

    static auto get_projection_matrix = [&](float eye_fov, float aspect_ratio, float zNear, float zFar) {
        eye_fov = degrees_to_radians(eye_fov);
        return Matrix4f{
            1/(tan(eye_fov/2)*aspect_ratio), 0               , 0                        , 0,
            0                              , 1/tan(eye_fov/2), 0                        , 0,
            0                              , 0               , (zFar+zNear)/(zFar-zNear), (2*zFar*zNear)/(zFar-zNear),
            0                              , 0               , -1                       , 0, 
        };
    };

    Vector3f angle{-10.0f, 20.0f, 0.0f};
    Vector3f eye_pos{0.0f, 0.0f, 5.0f};
    Matrix4f model = get_model_matrix({1.0f}, angle, {0.0f, 0.0f, 0.0f});
    Matrix4f view = get_view_matrix(eye_pos);
    Matrix4f projection = get_projection_matrix(45.0f, 1.0f, 0.1f, 50.0f);
    Matrix4f mvp = projection * view * model;

    for (auto& t : triangels) {
        // Triangle newTri = *t;

        // std::array<Vector4f, 3> mm {
        //         (view * model * t->vertices[0]),
        //         (view * model * t->vertices[1]),
        //         (view * model * t->vertices[2])
        // };

        // Vector3f view_pos[3];
        // for (int i = 0; i < 3; i++)
        //     view_pos[i] = mm[i].xyz();

        // Vector4f v[] = {
        //         mvp * t->vertices[0],
        //         mvp * t->vertices[1],
        //         mvp * t->vertices[2]
        // };
        // //Homogeneous division
        // for (int i = 0; i < 3; i++) {
        //     v[i].x /= v[i].w;
        //     v[i].y /= v[i].w;
        //     v[i].z /= v[i].w;
        // }

        // Matrix4f inv_trans = (view * model).inverse().transpose();
        // Vector4f n[] = {
        //         inv_trans * Vector4f(t->normals[0], 0.0f),
        //         inv_trans * Vector4f(t->normals[1], 0.0f),
        //         inv_trans * Vector4f(t->normals[2], 0.0f)
        // };

        // //Viewport transformation
        // for (int i = 0; i < 3; i++)
        // {
        //     v[i].x = 0.5*width*(v[i].x+1.0);
        //     v[i].y = 0.5*height*(v[i].y+1.0);
        //     v[i].z = v[i].z * f1 + f2;
        // }

        // for (int i = 0; i < 3; ++i)
        // {
        //     //screen space coordinates
        //     newTri.setVertices(i, v[i]);
        // }

        // for (int i = 0; i < 3; ++i)
        // {
        //     //view space normal
        //     newTri.setNormals(i, n[i].xyz().normalized());
        // }




        Triangle newTri = *t;
        Vector3f view_pos[3];
    
        for (int i = 0; i < 3; i++)
            vertex_shader({newTri.vertices[i], newTri.normals[i], view_pos[i]});

        static auto ViewPort = [](Vector4f& p, int w, int h) -> void {
            float f1 = (50 - 0.1) / 2.0;
            float f2 = (50 + 0.1) / 2.0;
            p.x = w*0.5f*(p.x+1.0f);
            p.y = h*(1.0f - 0.5f*(p.y+1.0f));
            p.z = p.z * f1 + f2;
        };

        for (int i = 0; i < 3; i++)
            ViewPort(newTri.vertices[i], width, height);

        draw_triangle_filled(&newTri, view_pos);
    }
}