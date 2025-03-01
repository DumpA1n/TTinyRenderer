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

Matrix4f get_model_matrix(const Vector3f scale, const Vector3f rotate, const Vector3f translate) {
    float cx = cos(rotate.x), sx = sin(rotate.x);
    float cy = cos(rotate.y), sy = sin(rotate.y);
    float cz = cos(rotate.z), sz = sin(rotate.z);
    return Matrix4f{
        scale.x * (cy * cz), scale.x * (cz * sx * sy - cx * sz), scale.x * (cx * cz * sy + sx * sz), translate.x,
        scale.y * (cy * sz), scale.y * (cx * cz + sx * sy * sz), scale.y * (cx * sy * sz - cz * sx), translate.y,
        scale.z * (-sy)    , scale.z * (cy * sx)               , scale.z * (cx * cy)               , translate.z,
        0.0f               , 0.0f                              , 0.0f                              , 1.0f
    };
}

Matrix4f get_view_matrix(const Vector3f eye_pos)
{
    Matrix4f view = Matrix4f::identity();
    Matrix4f translate{
        1, 0, 0, -eye_pos.x,
        0, 1, 0, -eye_pos.y,
        0, 0, 1, -eye_pos.z,
        0, 0, 0, 1
    };
    return translate * view;
}

Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar) {
    return Matrix4f{

    };
}

Vector3f default_vertex_shader(const vertex_shader_payload& payload) {
    Vector3f angle{0.0f, 140.0f, 0.0f};
    Vector3f eye_pos{0.0f, 0.0f, 10.0f};
    return get_model_matrix({1.0f}, angle, {0.0f}) * 
                get_view_matrix(eye_pos) * 
                    get_projection_matrix(45.0f, 1.0f, 0.1f, 50.0f) * payload.position;
}

Vector3f default_fragment_shader(const fragment_shader_payload& payload) {
    
    Vector3f ka{0.005, 0.005, 0.005};
    Vector3f kd = payload.color;
    Vector3f ks{0.7937, 0.7937, 0.7937};

    Light l1{{20, 20, 20}, {500, 500, 500}};
    Light l2{{-20, 20, 0}, {500, 500, 500}};

    std::vector<Light> lights = {l1, l2};
    Vector3f amb_light_intensity{10, 10, 10};
    Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Vector3f color = payload.color;
    Vector3f point = payload.view_pos;
    Vector3f normal = payload.normal;

    Vector3f result_color = {0, 0, 0};
    
    for (auto& light : lights)
    {
        Vector3f eye_dir = (eye_pos - point).normalized();
        Vector3f light_dir = (light.position - point).normalized();
        Vector3f normal_dir = normal;

        Vector3f I = light.intensity;
        float   r2 = (light.position - point).squaredNorm();
        Vector3f h = (eye_dir + light_dir).normalized();

        Vector3f ambient = ka.cwiseProduct(amb_light_intensity);

        Vector3f diffuse = kd.cwiseProduct(I / r2) * std::max(0.0f, normal_dir.dot(light_dir));

        Vector3f specular = ks.cwiseProduct(I / r2) * std::max(0.0f, std::pow(normal_dir.dot(h), p));

        result_color += ambient + diffuse + specular;
    }

    return result_color;
}

int main() {
    srand(0);
    Rasterizer rst(WIDTH, HEIGHT);
    rst.clear_buffer();

    rst.set_vertex_shader((void*)&default_vertex_shader);
    rst.set_fragment_shader((void*)&default_fragment_shader);

    std::vector<Triangle*> triangles;
    Model* obj = new Model();
    // obj->load("../../models/spot/spot_triangulated_good.obj");
    // obj->load("../../models/rock/rock.obj");
    // obj->load("../../models/Crate/Crate1.obj");
    obj->load("../../models/african_head/african_head.obj");
    for (int i = 0; i < obj->indices.size(); i += 3) {
        Triangle* tri = new Triangle();
        for (int j = 0; j < 3; j++) {
            tri->vertices[j] = obj->vertices[obj->indices[i + j]];
            tri->texCoords[j] = obj->texCoords[obj->indices[i + j]];
            tri->normals[j] = obj->normals[obj->indices[i + j]];

            // draw line
            // Vector3f v0 = obj->vertices[obj->indices[i + j]];
            // Vector3f v1 = obj->vertices[obj->indices[i + (j+1)%3]];
            // Vector3f p1{(int)((v0.x+1.)*WIDTH*.5), (int)((1.0 - (v0.y+1.)*0.5) * HEIGHT)};
            // Vector3f p2{(int)((v1.x+1.)*WIDTH*.5), (int)((1.0 - (v1.y+1.)*0.5) * HEIGHT)};            
            // rst.draw_line(p1, p2, {1.0f, 1.0f, 1.0f});
        }
        // draw triangle filled
        // std::vector<Vector3f> ts;
        // Vector3f v0 = obj->vertices[obj->indices[i + 0]];
        // Vector3f v1 = obj->vertices[obj->indices[i + 1]];
        // Vector3f v2 = obj->vertices[obj->indices[i + 2]];
        // Vector3f p0{(int)((v0.x+1.)*WIDTH*.5), (int)((1.0 - (v0.y+1.)*0.5) * HEIGHT)};
        // Vector3f p1{(int)((v1.x+1.)*WIDTH*.5), (int)((1.0 - (v1.y+1.)*0.5) * HEIGHT)};
        // Vector3f p2{(int)((v2.x+1.)*WIDTH*.5), (int)((1.0 - (v2.y+1.)*0.5) * HEIGHT)};
        // ts.push_back(p0);
        // ts.push_back(p1);
        // ts.push_back(p2);
        // Vector3f color = rst.fragment_shader({{1.0f}, tri->normals[0], tri->texCoords[0]});
        // rst.draw_triangle_fill(ts, color);

        rst.draw_triangle_filled(tri);
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