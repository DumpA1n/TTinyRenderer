#include <iostream>

#include "rasterizer.h"
#include "model.h"

#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <math.h>
#include <chrono>

#define WIDTH 700
#define HEIGHT 700

long long get_time() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

int main() {
    Rasterizer rst(WIDTH, HEIGHT);
    rst.clear_buffer();

    rst.set_vertex_shader((void*)&default_vertex_shader);
    rst.set_fragment_shader((void*)&default_fragment_shader);
    
    // rst.set_texture(new Texture("../../models/spot/spot_texture.png"));
    rst.set_texture(new Texture("../../models/african_head/african_head_diffuse.tga"));

    std::vector<Triangle*> triangles;
    Model* obj = new Model();
    // obj->load("../../models/spot/spot_triangulated_good.obj");
    obj->load("../../models/african_head/african_head.obj");
    for (int i = 0; i < obj->v_indices.size(); i += 3) {
        Triangle* tri = new Triangle();
        for (int j = 0; j < 3; j++) {
            tri->vertices[j] = obj->vertices[obj->v_indices[i + j]];
            tri->texCoords[j] = obj->texCoords[obj->vt_indices[i + j]];
            tri->normals[j] = obj->normals[obj->vn_indices[i + j]];
        }
        triangles.push_back(tri);
    }
    rst.draw(triangles);
    stbi_write_png("out.png", WIDTH, HEIGHT, 3, rst.get_stb_frame_buffer().data(), WIDTH * 3);

    return 0;
}