#pragma once

#include "object.h"

class Camera : public Object {
public:
    Camera(const Vector3f& pos = {0, 0, 0}, const Vector3f& rot = {0, 0, 0}, const Vector3f& s = {1, 1, 1})
        : Object(pos, rot, s) {}

    std::string name() override { return "Camera"; }
};