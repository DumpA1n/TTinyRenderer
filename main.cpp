#include <iostream>
#include <optional>

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

struct Shader {
    Model* m;
    std::vector<Triangle*> triangleList;
    std::unordered_map<std::string, Texture*> textureMap;
    Vector4f (*vertex_shader)(const vertex_shader_payload& payload);
    Vector3f (*fragment_shader)(const fragment_shader_payload& payload);

    Shader() {}
};

int main() {
    auto start = std::chrono::high_resolution_clock::now(); // 记录开始时间
    Rasterizer rst(WIDTH, HEIGHT, 4);
    Model obj;

    rst.set_vertex_shader((void*)&default_vertex_shader);

    std::string modelname = "spot";

#if defined(_MSC_VER)
    std::string FilesDir = "../../";
#elif defined(__GNUC__) || defined(__clang__)
    std::string FilesDir = "../";
#else
    std::string FilesDir = "../";
#endif

    if (modelname == "africa_head") {
        rst.set_fragment_shader((void*)&african_head_fragment_shader);
        rst.add_texture("texture", new Texture(FilesDir + "models/african_head/african_head_SSS.jpg"));
        rst.add_texture("diffuse", new Texture(FilesDir + "models/african_head/african_head_diffuse.tga"));
        rst.add_texture("specular", new Texture(FilesDir + "models/african_head/african_head_spec.tga"));
        rst.add_texture("normal", new Texture(FilesDir + "models/african_head/african_head_nm_tangent.tga"));
        obj.load(FilesDir + "models/african_head/african_head.obj");
    } else if (modelname == "spot") {
        rst.set_fragment_shader((void*)&phong_texture_fragment_shader);
        rst.set_texture(new Texture(FilesDir + "models/spot/spot_texture.png"));
        obj.load(FilesDir + "models/spot/spot_triangulated_good.obj");
    } else if (modelname == "diablo3_pose") {
        rst.set_fragment_shader((void*)&phong_texture_fragment_shader);
        rst.set_texture(new Texture(FilesDir + "models/diablo3_pose/diablo3_pose_diffuse.tga"));
        obj.load(FilesDir + "models/diablo3_pose/diablo3_pose.obj");
    }

    std::vector<Triangle*> triangles;
    for (int i = 0; i < obj.size(); i += 3) {
        Triangle* tri = new Triangle();
        for (int j = 0; j < 3; j++) {
            tri->setVertices(j, obj.getVertices(i + j));
            tri->setTexCoords(j, obj.getTexCoords(i + j));
            tri->setNormals(j, obj.getNormals(i + j));
        }
        triangles.push_back(tri);
    }

    while (1) {
        rst.clear_buffer({0.0f, 0.0f, 0.0f});

        rst.draw(triangles, RASTERIZE_MODE);

        Vector3f tmp;
        Vector4f zero  = Vector4f{Vector3f{0, 0, 0}, 1};
        Vector4f xAxis = Vector4f{normalized(Vector3f{1, 0, 0}), 1};
        Vector4f yAxis = Vector4f{normalized(Vector3f{0, 1, 0}), 1};
        Vector4f zAxis = Vector4f{normalized(Vector3f{0, 0, 1}), 1};
        Vector4f l1    = Vector4f{normalized(Vector3f{20, 20, 20}), 1};
        Vector4f l2    = Vector4f{normalized(Vector3f{-20, 20, 0}), 1};
        for (auto& it : {std::ref(zero), std::ref(xAxis), std::ref(yAxis), std::ref(zAxis), std::ref(l1), std::ref(l2)}) {
            rst.vertex_shader({it, tmp, tmp});
            rst.ViewPort(it, WIDTH, HEIGHT);
        }

        rst.draw_line(zero.xy(), xAxis.xy(), {1, 0, 0});
        rst.draw_line(zero.xy(), yAxis.xy(), {0, 1, 0});
        rst.draw_line(zero.xy(), zAxis.xy(), {0, 0, 1});

        rst.draw_line(zero.xy(), l1.xy(), {0.5f, 0.5f, 0.5f});
        rst.draw_line(zero.xy(), l2.xy(), {1, 1, 1});

        stbi_write_png("out.png", WIDTH, HEIGHT, rst.channels, rst.get_current_frame_buffer().data(), 0);
        break;
    }

    auto end = std::chrono::high_resolution_clock::now(); // 记录结束时间
    std::chrono::duration<double, std::milli> elapsed = end - start; // 计算耗时（毫秒）
    std::cout << "函数执行时间: " << elapsed.count() << " 毫秒\n";
    return 0;
}