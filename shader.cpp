// #include "shader.h"

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
