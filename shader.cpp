#include "shader.h"
#include "math.h"

#include <iostream>

#define PI 3.14159265358979323846f
inline float degrees_to_radians(float degree) {
    return degree * (PI / 180.0f);
}

Matrix4f get_model_matrix(const Vector3f& scale, const Vector3f& rotate, const Vector3f& translate) {
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
}

Matrix4f get_view_matrix(const Vector3f& eye_pos) {
    Vector3f up(0, 1, 0);
    Vector3f target(0, 0, 0);
    Vector3f z_axis = (eye_pos - target).normalized();
    Vector3f x_axis = up.cross(z_axis).normalized();
    Vector3f y_axis = z_axis.cross(x_axis);

    return Matrix4f{
        x_axis.x, x_axis.y, x_axis.z, -x_axis.dot(eye_pos),
        y_axis.x, y_axis.y, y_axis.z, -y_axis.dot(eye_pos),
        z_axis.x, z_axis.y, z_axis.z, -z_axis.dot(eye_pos),
        0,        0,        0,        1
    };
}

Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar) {
    eye_fov = degrees_to_radians(eye_fov);
    return Matrix4f{
        1/(tan(eye_fov/2)*aspect_ratio), 0               , 0                         , 0,
        0                              , 1/tan(eye_fov/2), 0                         , 0,
        0                              , 0               , (zNear+zFar)/(zNear-zFar), (2*zNear*zFar)/(zNear-zFar),
        0                              , 0               , -1                        , 0, 
    };
}
float angleY = -25.0f;
Vector4f default_vertex_shader(const vertex_shader_payload& payload) {
    Vector3f angle{5.0f, angleY, 0.0f};
    Vector3f eye_pos{0.0f, 0.0f, 3.0f};

    Matrix4f model = get_model_matrix({1.0f}, angle, {0.0f, 0.0f, 0.0f});
    Matrix4f view = get_view_matrix(eye_pos);
    Matrix4f projection = get_projection_matrix(45.0f, 1.0f, 0.1f, 50.0f);
    Matrix4f mvp = projection * view * model;
    Matrix4f viewmodel = view * model;

    payload.view_pos = (viewmodel * payload.position).xyz();

    payload.normal = (viewmodel.inverse().transpose() * Vector4f{payload.normal, 0.0f}).xyz().normalized();

    payload.position = mvp * payload.position;
    payload.position.x /= payload.position.w;
    payload.position.y /= payload.position.w;
    payload.position.z /= payload.position.w;

    return payload.position;
}

Vector3f default_fragment_shader(const fragment_shader_payload& payload) {
    return payload.color;
}

Vector3f texture_fragment_shader(const fragment_shader_payload& payload) {
    Vector3f texture_color{0.0f};
    if (payload.texture != nullptr)
        texture_color = payload.texture->sampler2D(payload.tex_coords.x, payload.tex_coords.y);
    return texture_color;
}

Vector3f african_head_fragment_shader(const fragment_shader_payload& payload) {
    const std::unordered_map<std::string, Texture*>& textures = payload.textureMap;
    
    Texture* tex_texture  = textures.count("texture")  ? textures.at("texture")  : nullptr;
    Texture* tex_specular = textures.count("specular") ? textures.at("specular") : nullptr;
    Texture* tex_diffuse  = textures.count("diffuse")  ? textures.at("diffuse")  : nullptr;
    Texture* tex_normal   = textures.count("normal")   ? textures.at("normal")   : nullptr;
    if (tex_normal == nullptr) {
        std::cerr << "texture normal not loaded!!!" << std::endl;
        return {0.0f};
    }

    static auto sample_or_default = [](Texture* tex, Vector2f coords) {
        return tex ? tex->sampler2D(coords) : Vector3f{0.0f};
    };
    
    Vector3f texture  = sample_or_default(tex_texture, payload.tex_coords);
    Vector3f specular = sample_or_default(tex_specular, payload.tex_coords);
    Vector3f diffuse  = sample_or_default(tex_diffuse, payload.tex_coords);
    Vector3f nm       = sample_or_default(tex_normal, payload.tex_coords);

    Vector3f kd = diffuse;
    Vector3f ks = specular;

    Light light{{5, 5, 5}};
    Vector3f eye_pos{0, 0, 10};
    float p = 150;

    Vector3f point = payload.view_pos;
    Vector3f normal = payload.normal;

    // 构建TBN矩阵
    Vector3f n = normal.normalized();
    float x = n.x, y = n.y, z = n.z;
    Vector3f t(x * y / sqrt(x * x + z * z), sqrt(x * x + z * z), z * y / sqrt(x * x + z * z));
    Vector3f b = n.cross(t);
    Matrix3f TBN{ t, b, n };

    // 法线贴图值从[0,1]映射到[-1,1]
    nm = nm * 2.0f - Vector3f{1.0f};
    normal = (TBN * nm).normalized();

    // 计算光照
    Vector3f light_dir = (light.position - point).normalized();
    Vector3f view_dir = (eye_pos - point).normalized();
    Vector3f half_vector = (light_dir + view_dir).normalized();

    float diff = std::max(normal.dot(light_dir), 0.0f);
    float spec = std::pow(std::max(normal.dot(half_vector), 0.0f), p);

    Vector3f ambient = kd * 0.15f;
    Vector3f diffuse_component = kd * diff;
    Vector3f specular_component = ks * spec;

    Vector3f result_color = ambient + diffuse_component + specular_component;

    result_color = {
        std::min(result_color.x, 1.0f),
        std::min(result_color.y, 1.0f),
        std::min(result_color.z, 1.0f)
    };

    return result_color;
}


Vector3f phong_texture_fragment_shader(const fragment_shader_payload& payload) {
    Vector3f texture_color{0.0f};
    if (payload.texture != nullptr)
        texture_color = payload.texture->sampler2D(payload.tex_coords.x, payload.tex_coords.y);

    Vector3f ka{0.005, 0.005, 0.005};
    Vector3f kd = texture_color;
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
        Vector3f normal_dir = normal.normalized();

        Vector3f I = light.intensity;
        float   r2 = (light.position - point).squaredNorm();
        Vector3f h = (eye_dir + light_dir).normalized();

        Vector3f ambient = ka.cwiseProduct(amb_light_intensity);

        Vector3f diffuse = kd.cwiseProduct(I / r2) * std::max(0.0f, normal_dir.dot(light_dir));

        Vector3f specular = ks.cwiseProduct(I / r2) * std::max(0.0f, std::pow(normal_dir.dot(h), p));

        result_color += ambient + diffuse + specular;
    }

    result_color = {
        result_color.x > 1.0f ? 1.0f : result_color.x,
        result_color.y > 1.0f ? 1.0f : result_color.y,
        result_color.z > 1.0f ? 1.0f : result_color.z
    };

    return result_color;
}

Vector3f phong_fragment_shader(const fragment_shader_payload& payload) {
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

    result_color = {
        result_color.x > 1.0f ? 1.0f : result_color.x,
        result_color.y > 1.0f ? 1.0f : result_color.y,
        result_color.z > 1.0f ? 1.0f : result_color.z
    };

    return result_color;
}











// Vector3f african_head_fragment_shader(const fragment_shader_payload& payload) {
//     const std::unordered_map<std::string, Texture*>& textures = payload.textureMap;
    
//     Texture* tex_texture  = textures.count("texture")  ? textures.at("texture")  : nullptr;
//     Texture* tex_specular = textures.count("specular") ? textures.at("specular") : nullptr;
//     Texture* tex_diffuse  = textures.count("diffuse")  ? textures.at("diffuse")  : nullptr;
//     Texture* tex_normal   = textures.count("normal")   ? textures.at("normal")   : nullptr;
//     if (tex_normal == nullptr) {
//         std::cerr << "texture normal not loaded!!!" << std::endl;
//         return {0.0f};
//     }

//     static auto sample_or_default = [](Texture* tex, Vector2f coords) {
//         return tex ? tex->sampler2D(coords) : Vector3f{0.0f};
//     };
    
//     Vector3f texture  = sample_or_default(tex_texture, payload.tex_coords);
//     Vector3f specular = sample_or_default(tex_specular, payload.tex_coords);
//     Vector3f diffuse  = sample_or_default(tex_diffuse, payload.tex_coords);
//     Vector3f nm       = sample_or_default(tex_normal, payload.tex_coords);

//     Vector3f ka{0.005, 0.005, 0.005};
//     Vector3f kd = diffuse;
//     Vector3f ks = specular;

//     Light l1{{20, 20, 20}, {500, 500, 500}};
//     Light l2{{-20, 20, 0}, {500, 500, 500}};

//     std::vector<Light> lights = {l1, l2};
//     Vector3f amb_light_intensity{10, 10, 10};
//     Vector3f eye_pos{0, 0, 10};

//     float p = 150;

//     Vector3f color = payload.color;
//     Vector3f point = payload.view_pos;
//     Vector3f normal = payload.normal;

//     float kh = 0.2, kn = 5;
//     auto n = nm;
//     auto x = n.x, y = n.y, z = n.z;
//     auto u = payload.tex_coords.x;
//     auto v = payload.tex_coords.y;
//     float w = (float)tex_normal->width;
//     float h = (float)tex_normal->height;
//     Vector3f t(x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z));
//     Vector3f b = n.cross(t);
//     Matrix3f TBN{ t, b, n };
//     auto dU = kh * kn * (tex_normal->sampler2D(u+1/w,v).norm()-tex_normal->sampler2D(u,v).norm());
//     auto dV = kh * kn * (tex_normal->sampler2D(u,v+1/h).norm()-tex_normal->sampler2D(u,v).norm());
//     Vector3f ln(-dU, -dV, 1);

//     // bump mapping
//     normal = (TBN * ln).normalized();
    
//     // displacement mapping
//     point += n * kn * tex_normal->sampler2D(u,v).norm();

//     Vector3f result_color = {0, 0, 0};
    
//     for (auto& light : lights)
//     {
//         Vector3f eye_dir = (eye_pos - point).normalized();
//         Vector3f light_dir = (light.position - point).normalized();
//         Vector3f normal_dir = normal.normalized();

//         Vector3f I = light.intensity;
//         float   r2 = (light.position - point).squaredNorm();
//         Vector3f h = (eye_dir + light_dir).normalized();

//         Vector3f ambient = ka.cwiseProduct(amb_light_intensity);

//         Vector3f diffuse = kd.cwiseProduct(I / r2) * std::max(0.0f, normal_dir.dot(light_dir));

//         Vector3f specular = ks.cwiseProduct(I / r2) * std::max(0.0f, std::pow(normal_dir.dot(h), p));

//         result_color += diffuse + specular;
//     }

//     result_color = {
//         result_color.x > 1.0f ? 1.0f : result_color.x,
//         result_color.y > 1.0f ? 1.0f : result_color.y,
//         result_color.z > 1.0f ? 1.0f : result_color.z
//     };

//     return result_color;
// }


// Vector3f african_head_fragment_shader(const fragment_shader_payload& payload) {
//     const std::unordered_map<std::string, Texture*>& textures = payload.textureMap;
    
//     Texture* tex_texture  = textures.count("texture")  ? textures.at("texture")  : nullptr;
//     Texture* tex_specular = textures.count("specular") ? textures.at("specular") : nullptr;
//     Texture* tex_diffuse  = textures.count("diffuse")  ? textures.at("diffuse")  : nullptr;
//     Texture* tex_normal   = textures.count("normal")   ? textures.at("normal")   : nullptr;
//     if (tex_normal == nullptr) {
//         std::cerr << "texture normal not loaded!!!" << std::endl;
//         return {0.0f};
//     }

//     static auto sample_or_default = [](Texture* tex, Vector2f coords) {
//         return tex ? tex->sampler2D(coords) : Vector3f{0.0f};
//     };
    
//     Vector3f texture  = sample_or_default(tex_texture, payload.tex_coords);
//     Vector3f specular = sample_or_default(tex_specular, payload.tex_coords);
//     Vector3f diffuse  = sample_or_default(tex_diffuse, payload.tex_coords);
//     Vector3f nm       = sample_or_default(tex_normal, payload.tex_coords);

//     Vector3f ka{0.005, 0.005, 0.005};
//     Vector3f kd = diffuse;
//     Vector3f ks = specular;

//     Light l1{{20, 20, 20}, {500, 500, 500}};
//     Light l2{{-20, 20, 0}, {500, 500, 500}};

//     std::vector<Light> lights = {l1};
//     Vector3f amb_light_intensity{10, 10, 10};
//     Vector3f eye_pos{0, 0, 3};

//     float p = 150;

//     Vector3f color = payload.color;
//     Vector3f point = payload.view_pos;
//     Vector3f normal = payload.normal;

//     float kh = 0.2, kn = 0.1;
//     auto n = normal;
//     auto x = n.x, y = n.y, z = n.z;
//     Vector3f t(x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z));
//     Vector3f b = n.cross(t);
//     Matrix3f TBN{ t, b, n };

//     auto u = payload.tex_coords.x;
//     auto v = payload.tex_coords.y;
//     float w = (float)tex_normal->width;
//     float h = (float)tex_normal->height;
//     auto dU = kh * kn * (tex_normal->sampler2D(u+1/w,v).norm()-tex_normal->sampler2D(u,v).norm());
//     auto dV = kh * kn * (tex_normal->sampler2D(u,v+1/h).norm()-tex_normal->sampler2D(u,v).norm());
//     Vector3f ln(-dU, -dV, 1);

//     // bump mapping
//     normal = (TBN * nm).normalized() * 2.0f;
    
//     // displacement mapping
//     point += tex_normal->sampler2D(u,v) * 2.0f;

//     Vector3f result_color = {0, 0, 0};
    
//     for (auto& light : lights)
//     {
//         Vector3f eye_dir = (eye_pos - point).normalized();
//         Vector3f light_dir = (light.position - point).normalized();
//         Vector3f normal_dir = normal.normalized();

//         Vector3f I = light.intensity;
//         float   r2 = (light.position - point).squaredNorm();
//         Vector3f h = (eye_dir + light_dir).normalized();

//         Vector3f ambient = ka.cwiseProduct(amb_light_intensity);

//         Vector3f diffuse = kd * std::max(0.0f, normal_dir.dot(light_dir));

//         Vector3f specular = ks * std::max(0.0f, std::pow(normal_dir.dot(h), p));

//         result_color += diffuse + specular;
//     }

//     result_color = {
//         result_color.x > 1.0f ? 1.0f : result_color.x,
//         result_color.y > 1.0f ? 1.0f : result_color.y,
//         result_color.z > 1.0f ? 1.0f : result_color.z
//     };

//     return result_color;
// }