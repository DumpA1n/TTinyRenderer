#include <jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <cstring> // 用于 memcpy
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <thread>
#include <queue>

#include "rasterizer.h"
#include "model.h"
#include "utils.h"
#include "mlog.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int WIDTH = 700;
int HEIGHT = 700;

const static std::string FilesDir = "/data/data/com.example.mixture/files/";

std::atomic<bool> bIsRendering(false);

std::thread renderThread;

std::mutex renderMutex;

extern "C" JNIEXPORT void JNICALL
Java_com_example_tinyrenderer_NativeLib_startRender(
        JNIEnv* env,
        jobject /* this */,
        jobject surface,
        jstring modelName) {

    std::lock_guard<std::mutex> lock(renderMutex);

    if (renderThread.joinable()) {
        bIsRendering = false;
        renderThread.join();
    }

    bIsRendering = true;

    // 创建 ANativeWindow
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    if (!window) {
        LOGE("ANativeWindow_fromSurface failed");
        return;
    }

    // 设置缓冲区参数
    ANativeWindow_setBuffersGeometry(window, WIDTH, HEIGHT, WINDOW_FORMAT_RGBA_8888);

    const char* chars = env->GetStringUTFChars(modelName, nullptr);
    std::string modelname(chars);
    env->ReleaseStringUTFChars(modelName, chars);

    LOGI("model name: %s", modelname.c_str());

    // **启动渲染线程**
    renderThread = std::thread([window, modelname]() {
        Rasterizer rst(WIDTH, HEIGHT, 4);
        Model obj;

        rst.set_vertex_shader((void*)&default_vertex_shader);

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

        while (bIsRendering) {
            auto start = std::chrono::high_resolution_clock::now();

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

            // 锁定缓冲区
            ANativeWindow_Buffer buffer;
            if (ANativeWindow_lock(window, &buffer, nullptr) != 0) {
                LOGE("ANativeWindow_lock failed");
                ANativeWindow_release(window);
                return;
            }

            uint8_t *dst = (uint8_t *) buffer.bits;

            // LOGI("width: %d   stride: %d", WIDTH, buffer.stride);

            for (int y = 0; y < HEIGHT; y++) {
                memcpy(dst + buffer.stride * y * rst.channels, &rst.current_frame_buffer[WIDTH * y * rst.channels], WIDTH * rst.channels);
            }

            ANativeWindow_unlockAndPost(window);

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start; // 毫秒
            LOGI("渲染耗时: %f 毫秒", elapsed.count());

            std::this_thread::sleep_for(std::chrono::milliseconds(33)); // 30 FPS
        }

        stbi_write_png((FilesDir + "out.png").c_str(), WIDTH, HEIGHT, rst.channels, rst.get_current_frame_buffer().data(), 0);

        ANativeWindow_release(window);
    });

    renderThread.detach();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_tinyrenderer_NativeLib_stopRender(JNIEnv* env, jobject /* this */, jobject surface) {
    std::lock_guard<std::mutex> lock(renderMutex);

    bIsRendering = false;

    if (renderThread.joinable()) {
        renderThread.join();
    }

    // 创建 ANativeWindow
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    if (!window) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "ANativeWindow_fromSurface failed");
        return;
    }

    // 设置缓冲区参数
    ANativeWindow_setBuffersGeometry(window, WIDTH, HEIGHT, WINDOW_FORMAT_RGBA_8888);

    // 锁定缓冲区
    ANativeWindow_Buffer buffer;
    if (ANativeWindow_lock(window, &buffer, nullptr) != 0) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "ANativeWindow_lock failed");
        ANativeWindow_release(window);
        return;
    }

    uint8_t* dst = (uint8_t*)buffer.bits;

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int dst_index = (y * buffer.stride + x) * 4;  // RGBA
            for (int i : {0, 1, 2, 3})
                dst[dst_index + i] = 0;
        }
    }

    ANativeWindow_unlockAndPost(window);

    ANativeWindow_release(window);
}

extern "C" JNIEXPORT int JNICALL
Java_com_example_tinyrenderer_NativeLib_isRendering(JNIEnv* env, jobject /* this */) {
    return bIsRendering;
}
