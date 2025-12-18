#pragma once

#include <vector>

#include "object.h"
#include "utils/Singleton.h"

class ObjectManager : public Singleton<ObjectManager> {
    friend class Singleton<ObjectManager>;

public:
    void initialize();

    auto& objects() const { return objects_; }

private:
    std::vector<Object::Ptr> objects_;
};
