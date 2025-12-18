#pragma once

#include <memory>
#include <vector>

#include "object/object.h"
#include "utils/ThreadPool.h"
#include "utils/mmath.h"
#include "utils/triangle.h"

class Texture;
class IShader;
class Scene;

class Rasterizer {
public:
    enum ERenderMode {
        TEXTURE_MODE = 1,
        LINE_FRAME_MODE = 2,
        PURE_COLOR_MODE = 3
    };

    struct RenderParams {
        ERenderMode render_mode = TEXTURE_MODE;
        bool msaa4x = true;
        bool ssaa4x = false;
        bool multi_thread = false;
    };

    Rasterizer(int width, int height, int channels);

    void initialize();

    void run();
    void run_multi_thread();

    void rasterize(const std::shared_ptr<Object> obj, const Triangle& t, const std::array<Vector3f, 3>& view_pos);

    void draw_line(const Vector2f& p1, const Vector2f& p2, const Vector3f& col);
    void draw_triangle(const Triangle& t, const Vector3f& col);
    void draw_triangle_filled(const Triangle& t, const Vector3f& col);

    void set_render_params(const RenderParams& params) { render_params_ = params; }

    int width()    const { return width_; }
    int height()   const { return height_; }
    int channels() const { return channels_; }

    int get_index(int x, int y) const;

    void set_pixel(const Vector2i& p, const Vector3f& col);
    void set_pixel(int x, int y, const Vector3f& col);

    void clear_buffer(const Vector3f& col);

    auto& get_current_frame_buffer() const { return current_frame_buffer_; }
    auto& get_last_frame_buffer() const { return last_frame_buffer_; }

    auto scene() const { return scene_; }
    void set_scene(const std::shared_ptr<Scene> scene) { scene_ = scene; }

    auto& get_model_matrix() { return model_matrix_; }
    void  set_model_matrix(const Vector3f& scale, const Vector3f& rotate, const Vector3f& translate);

    auto& get_view_matrix() { return view_matrix_; }
    void  set_view_matrix(const Vector3f& eye_pos);

    auto& get_projection_matrix() { return projection_matrix_; }
    void  set_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar);

    inline static float degrees_to_radians(float degree) {
        return degree * (M_PI / 180.0f);
    }

    static void ViewPort(Vector4f& p, int w, int h);
    static std::tuple<float, float, float> ComputeBarycentric2D(float x, float y, const std::array<Vector4f, 3>& v);
    static Vector3f Interpolate(float alpha, float beta, float gamma, const Vector3f& vert1, const Vector3f& vert2, const Vector3f& vert3, float weight);
    static Vector3f Interpolate(float alpha, float beta, float gamma, const std::array<Vector3f, 3>& vert, float weight);
    static Vector2f Interpolate(float alpha, float beta, float gamma, const Vector2f& vert1, const Vector2f& vert2, const Vector2f& vert3, float weight);
    static Vector2f Interpolate(float alpha, float beta, float gamma, const std::array<Vector2f, 3>& vert, float weight);
    static bool IsInsideTriangle2D(const Vector3f& p, const Triangle& t);
    static bool IsInsideTriangle2D(float x, float y, const Triangle& t);

private:
    int width_;
    int height_;
    int channels_;

    std::shared_ptr<Scene> scene_ = nullptr;

    std::vector<uint8_t> current_frame_buffer_;
    std::vector<std::vector<Vector3f>> current_frame_buffer_4x_;
    std::vector<uint8_t> last_frame_buffer_;
    std::vector<float> depth_buffer_;
    std::vector<std::vector<float>> depth_buffer_4x_;

    RenderParams render_params_;

    Matrix4f model_matrix_;
    Matrix4f view_matrix_;
    Matrix4f projection_matrix_;
};