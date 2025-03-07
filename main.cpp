#include <iostream>

#include "rasterizer.h"
#include "model.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int WIDTH = 700;
int HEIGHT = 700;

int main() {
    Rasterizer rst(WIDTH, HEIGHT);
    rst.clear_buffer();

    rst.set_vertex_shader((void*)&default_vertex_shader);
    rst.set_fragment_shader((void*)&phong_texture_fragment_shader);
    
    rst.set_texture(new Texture("../../models/spot/spot_texture.png"));
    // rst.set_texture(new Texture("../../models/african_head/african_head_SSS.jpg"));
    // rst.set_texture(new Texture("../../models/african_head/african_head_diffuse.tga"));
    // rst.set_texture(new Texture("../../models/african_head/african_head_spec.tga"));
    // rst.set_texture(new Texture("../../models/african_head/african_head_nm.tga"));

    std::vector<Triangle*> triangles;
    Model* obj = new Model();
    obj->load("../../models/spot/spot_triangulated_good.obj");
    // obj->load("../../models/african_head/african_head.obj");
    for (int i = 0; i < obj->v_indices.size(); i += 3) {
        Triangle* tri = new Triangle();
        for (int j = 0; j < 3; j++) {
            tri->setVertices(j, obj->vertices[obj->v_indices[i + j]]);
            tri->setTexCoords(j, obj->tex_coords[obj->vt_indices[i + j]]);
            tri->setNormals(j, obj->normals[obj->vn_indices[i + j]]);
        }
        triangles.push_back(tri);
    }
    rst.draw(triangles);

    static auto ViewPort = [](Vector4f& p, int w, int h) -> void {
        float f1 = (50 - 0.1) / 2.0;
        float f2 = (50 + 0.1) / 2.0;
        p.x = w*0.5f*(p.x+1.0f);
        p.y = h*(1.0f - 0.5f*(p.y+1.0f));
        p.z = p.z * f1 + f2;
    };

    Vector4f zero  = rst.vertex_shader({Vector4f{0, 0, 0, 1}, Vector3f{}, Vector3f{}});
    Vector4f xAxis = rst.vertex_shader({Vector4f{1, 0, 0, 1}, Vector3f{}, Vector3f{}});
    Vector4f yAxis = rst.vertex_shader({Vector4f{0, 1, 0, 1}, Vector3f{}, Vector3f{}});
    Vector4f zAxis = rst.vertex_shader({Vector4f{0, 0, 1, 1}, Vector3f{}, Vector3f{}});
    ViewPort(zero, WIDTH, HEIGHT);
    ViewPort(xAxis, WIDTH, HEIGHT);
    ViewPort(yAxis, WIDTH, HEIGHT);
    ViewPort(zAxis, WIDTH, HEIGHT);
    rst.draw_line(zero.xy(), xAxis.xy(), {1, 0, 0});
    rst.draw_line(zero.xy(), yAxis.xy(), {0, 1, 0});
    rst.draw_line(zero.xy(), zAxis.xy(), {0, 0, 1});

    Vector4f l1 = rst.vertex_shader({Vector4f{20, 20, 20, 1}, Vector3f{}, Vector3f{}});
    Vector4f l2 = rst.vertex_shader({Vector4f{-20, 20, 0, 1}, Vector3f{}, Vector3f{}});
    ViewPort(l1, WIDTH, HEIGHT);
    ViewPort(l2, WIDTH, HEIGHT);
    rst.draw_line(zero.xy(), l1.xy(), {0.5f, 0.5f, 0.5f});
    rst.draw_line(zero.xy(), l2.xy(), {1, 1, 1});

    stbi_write_png("out.png", WIDTH, HEIGHT, 3, rst.get_stb_frame_buffer().data(), WIDTH * 3);

    return 0;
}