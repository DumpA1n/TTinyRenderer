#include <iostream>

#include "rasterizer.h"
#include "model.h"

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

// Matrix4f rotateX(float theta) {
//     float c = cos(theta);
//     float s = sin(theta);
//     return Matrix4f(
//         Vector4f{1.0f,  0.0f,  0.0f, 0.0f},
//         Vector4f{0.0f,     c,    -s, 0.0f},
//         Vector4f{0.0f,     s,     c, 0.0f},
//         Vector4f{0.0f,  0.0f,  0.0f, 1.0f}
//     );
// }

// Matrix4f rotateY(float theta) {
//     float c = cos(theta);
//     float s = sin(theta);
//     return Matrix4f{
//         Vector4f{    c,  0.0f,     s, 0.0f},
//         Vector4f{ 0.0f,  1.0f,  0.0f, 0.0f},
//         Vector4f{   -s,  0.0f,     c, 0.0f},
//         Vector4f{ 0.0f,  0.0f,  0.0f, 1.0f}
//     };
// }

// Matrix4f rotateZ(float theta) {
//     float c = cos(theta);
//     float s = sin(theta);
//     return Matrix4f{
//         Vector4f{    c,    -s,  0.0f, 0.0f},
//         Vector4f{    s,     c,  0.0f, 0.0f},
//         Vector4f{ 0.0f,  0.0f,  1.0f, 0.0f},
//         Vector4f{ 0.0f,  0.0f,  0.0f, 1.0f}
//     };
// }

// Matrix4f get_transform_matrix(Vector3f scale, Vector3f rotate, Vector3f model) {
//     Matrix4f mscale{
//         Vector4f{scale.x, 0.0f, 0.0f, 0.0f},
//         Vector4f{0.0f, scale.y, 0.0f, 0.0f},
//         Vector4f{0.0f, 0.0f, scale.z, 0.0f},
//         Vector4f{0.0f, 0.0f, 0.0f, 1.0f}
//     };
//     Matrix4f mrotate = rotateX(rotate.x) * rotateY(rotate.y) * rotateZ(rotate.z);
//     Matrix4f mmodel{
//         Vector4f{1.0f, 0.0f, 0.0f, model.x},
//         Vector4f{0.0f, 1.0f, 0.0f, model.y},
//         Vector4f{0.0f, 0.0f, 1.0f, model.z},
//         Vector4f{0.0f, 0.0f, 0.0f, 1.0f}
//     };
//     return mmodel * mrotate * mscale;
// }

Matrix4f get_transform_matrix(Vector3f scale, Vector3f rotate, Vector3f model) {
    float cx = cos(rotate.x), sx = sin(rotate.x);
    float cy = cos(rotate.y), sy = sin(rotate.y);
    float cz = cos(rotate.z), sz = sin(rotate.z);
    return Matrix4f{
        Vector4f{scale.x * (cy * cz), scale.x * (cz * sx * sy - cx * sz), scale.x * (cx * cz * sy + sx * sz), model.x},
        Vector4f{scale.y * (cy * sz), scale.y * (cx * cz + sx * sy * sz), scale.y * (cx * sy * sz - cz * sx), model.y},
        Vector4f{scale.z * (-sy)    , scale.z * (cy * sx)               , scale.z * (cx * cy)               , model.z},
        Vector4f{0.0f               , 0.0f                              , 0.0f                              , 1.0f}
    };
}

Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar) {
    return Matrix4f{

    };
}

Vector3f default_vertex_shader(const vertex_shader_payload& payload) {
    return get_transform_matrix(Vector3f{1.0f}, {10.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 5.0f}) * payload.position;
}

Vector3f default_fragment_shader(const fragment_shader_payload& payload) {
    return payload.color;
}

int main() {
    Rasterizer rst(WIDTH, HEIGHT);
    rst.clear_buffer();

    rst.set_vertex_shader((void*)&default_vertex_shader);
    rst.set_fragment_shader((void*)&default_fragment_shader);

    std::vector<Triangle*> triangles;
    Model* obj = new Model();
    obj->load("../../models/spot/spot_triangulated_good.obj");
    // obj->load("../../models/rock/rock.obj");
    // obj->load("../../models/Crate/Crate1.obj");
    // obj->load("../../models/african_head/african_head.obj");
    for (int i = 0; i < obj->indices.size(); i += 3) {
        Triangle* tri = new Triangle();
        for (int j = 0; j < 3; j++) {
            tri->vertices[j] = obj->vertices[obj->indices[i + j]];
            tri->texCoords[j] = obj->texCoords[obj->indices[i + j]];
            tri->normals[j] = obj->normals[obj->indices[i + j]];

            Vector3f v0 = obj->vertices[obj->indices[i + j]];
            Vector3f v1 = obj->vertices[obj->indices[i + (j+1)%3]];
            Vector3f p1{(int)((v0.x+1.)*WIDTH*.5), (int)((1.0 - (v0.y+1.)*0.5) * HEIGHT)};
            Vector3f p2{(int)((v1.x+1.)*WIDTH*.5), (int)((1.0 - (v1.y+1.)*0.5) * HEIGHT)};            
            rst.draw_line(p1, p2, {0.0f, 0.0f, 0.0f});
        }
        std::vector<Vector3f> ts;
        Vector3f v0 = obj->vertices[obj->indices[i + 0]];
        Vector3f v1 = obj->vertices[obj->indices[i + 1]];
        Vector3f v2 = obj->vertices[obj->indices[i + 2]];
        Vector3f p0{(int)((v0.x+1.)*WIDTH*.5), (int)((1.0 - (v0.y+1.)*0.5) * HEIGHT)};
        Vector3f p1{(int)((v1.x+1.)*WIDTH*.5), (int)((1.0 - (v1.y+1.)*0.5) * HEIGHT)};
        Vector3f p2{(int)((v2.x+1.)*WIDTH*.5), (int)((1.0 - (v2.y+1.)*0.5) * HEIGHT)};
        ts.push_back(p0);
        ts.push_back(p1);
        ts.push_back(p2);
        rst.draw_triangle_fill(ts, {1.0f, 1.0f, 1.0f});
        triangles.push_back(tri);
    }
    stbi_write_png("out.png", WIDTH, HEIGHT, 3, rst.get_stb_frame_buffer().data(), WIDTH * 3);

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

    // Vector3f color{1.0f, 1.0f, 1.0f};
    // rst.draw_line(Vector3f{1, 124}, Vector3f{142, 33}, color);
    // std::vector<Vector3f> pos{{100, 100}, {50, 50}, {150, 50}};
    // rst.draw_triangle(pos, color);
    // rst.draw_triangle_fill(pos, color);
    // stbi_write_png("out.png", WIDTH, HEIGHT, 3, rst.get_stb_frame_buffer().data(), WIDTH * 3);

    return 0;
}