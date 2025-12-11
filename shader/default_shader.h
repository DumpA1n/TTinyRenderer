#include "ishader.h"
#include "mmath.h"

class DefaultShader : public IShader {
public:
    DefaultShader() = default;

    vertex_shader_output_i vertex_shader(const vertex_shader_payload_i& payload) override {
        float angleY = -25.0f;
        Vector3f angle{5.0f, angleY, 0.0f};
        Vector3f eye_pos{0.0f, 0.0f, 3.0f};

        Matrix4f model = get_model_matrix({1.0f}, angle, {0.0f, 0.0f, 0.0f});
        Matrix4f view = get_view_matrix(eye_pos);
        Matrix4f projection = get_projection_matrix(45.0f, 1.0f, 0.1f, 50.0f);
        Matrix4f mvp = projection * view * model;
        Matrix4f viewmodel = view * model;

        vertex_shader_output_i output;

        // Compute view position
        output.view_pos = (viewmodel * payload.position).xyz();

        // Transform normal with the inverse transpose of the viewmodel matrix
        output.normal = normalized((viewmodel.inverse().transpose() * Vector4f{payload.normal, 0.0f}).xyz());

        // NDC coordinates
        Vector4f position = mvp * payload.position;
        position.x /= position.w;
        position.y /= position.w;
        position.z /= position.w;

        output.position = position;

        return output;
    }

    fragment_shader_output_i fragment_shader(const fragment_shader_payload_i& payload) override {
        fragment_shader_output_i output;

        output.color = payload.color;

        return output;
    }

};
