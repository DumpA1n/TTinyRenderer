#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "mmath.h"

class Model {
public:
    std::vector<Vector3f> vertices;   // v  顶点坐标
    std::vector<Vector3f> texCoords;  // vt 纹理坐标
    std::vector<Vector3f> normals;    // vn 法线
    std::vector<int> indices;         // f  索引

    void load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "无法打开文件：" << filename << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string type;
            iss >> type;

            // 顶点 v
            if (type == "v") {
                Vector3f v;
                iss >> v.x >> v.y >> v.z;
                vertices.push_back(v);
            }
            // 纹理坐标 vt
            else if (type == "vt") {
                Vector3f vt;
                iss >> vt.x >> vt.y;
                texCoords.push_back(vt);
            }
            // 法线 vn
            else if (type == "vn") {
                Vector3f vn;
                iss >> vn.x >> vn.y >> vn.z;
                normals.push_back(vn);
            }
            // 面 f
            else if (type == "f") {
                std::string vertexInfo;
                while (iss >> vertexInfo) {
                    std::replace(vertexInfo.begin(), vertexInfo.end(), '/', ' '); // 替换 '/' 为 ' '
                    std::istringstream vss(vertexInfo);
                    int vIdx, vtIdx, vnIdx;
                    vss >> vIdx >> vtIdx >> vnIdx;
                    indices.push_back(vIdx - 1);  // OBJ 索引是从 1 开始，C++ 需要 -1
                }
            }
        }
        file.close();
    }
};