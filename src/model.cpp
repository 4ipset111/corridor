#include "model.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

Model::Model(const std::string& path, const std::vector<std::string>& texturePaths) : VAO(0), VBO(0), vertexCount(0) {
    std::vector<float> vertices;
    if (loadOBJ(path, texturePaths, vertices)) {
        vertexCount = (GLsizei)(vertices.size() / 9);
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glBindVertexArray(0);
    }
}

Model::~Model() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Model::render() const {
    if (VAO) {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }
}

bool Model::loadOBJ(const std::string& path, const std::vector<std::string>& texturePaths, std::vector<float>& outVertices) {
    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec2> temp_texcoords;
    std::vector<glm::vec3> temp_normals;
    std::map<std::string, float> materialMap;

    float currentTexID = -1.0f;
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << path << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            temp_positions.push_back(pos);
        } else if (prefix == "vt") {
            glm::vec2 tex;
            ss >> tex.x >> tex.y;
            temp_texcoords.push_back(tex);
        } else if (prefix == "vn") {
            glm::vec3 norm;
            ss >> norm.x >> norm.y >> norm.z;
            temp_normals.push_back(norm);
        } else if (prefix == "mtllib") {
            std::string mtlFile;
            ss >> mtlFile;
            std::string directory = "";
            size_t lastSlash = path.find_last_of("/\\");
            if (lastSlash != std::string::npos) directory = path.substr(0, lastSlash + 1);
            loadMTL(directory + mtlFile, texturePaths, materialMap);
        } else if (prefix == "usemtl") {
            std::string matName;
            ss >> matName;
            if (materialMap.count(matName)) {
                currentTexID = materialMap.at(matName);
            }
        } else if (prefix == "f") {
            for (int i = 0; i < 3; i++) {
                std::string vertexData;
                ss >> vertexData;
                
                unsigned int pIdx = 0, tIdx = 0, nIdx = 0;
                size_t firstSlash = vertexData.find('/');
                size_t lastSlash = vertexData.rfind('/');

                pIdx = std::stoi(vertexData.substr(0, firstSlash));
                
                if (firstSlash != std::string::npos && lastSlash != std::string::npos) {
                    if (lastSlash > firstSlash + 1) {
                        tIdx = std::stoi(vertexData.substr(firstSlash + 1, lastSlash - firstSlash - 1));
                    }
                    nIdx = std::stoi(vertexData.substr(lastSlash + 1));
                } else if (firstSlash != std::string::npos) {
                    tIdx = std::stoi(vertexData.substr(firstSlash + 1));
                }

                glm::vec3 p = temp_positions[pIdx - 1];
                glm::vec2 t = (tIdx > 0) ? temp_texcoords[tIdx - 1] : glm::vec2(0.0f);
                glm::vec3 n = (nIdx > 0) ? temp_normals[nIdx - 1] : glm::vec3(0.0f, 1.0f, 0.0f);

                outVertices.push_back(p.x); outVertices.push_back(p.y); outVertices.push_back(p.z);
                outVertices.push_back(t.x); outVertices.push_back(t.y);
                outVertices.push_back(n.x); outVertices.push_back(n.y); outVertices.push_back(n.z);
                outVertices.push_back(currentTexID);
            }
        }
    }
    return true;
}

void Model::loadMTL(const std::string& mtlPath, const std::vector<std::string>& texturePaths, std::map<std::string, float>& outMaterialMap) {
    std::ifstream file(mtlPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open MTL file: " << mtlPath << std::endl;
        return;
    }

    std::string line, currentMtlName;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "newmtl") {
            ss >> currentMtlName;
        } else if (prefix == "map_Kd") {
            std::string texFile;
            ss >> texFile;
            size_t lastSlash = texFile.find_last_of("/\\");
            if (lastSlash != std::string::npos) texFile = texFile.substr(lastSlash + 1);

            for (size_t i = 0; i < texturePaths.size(); ++i) {
                std::string p = texturePaths[i];
                size_t pLastSlash = p.find_last_of("/\\");
                if (pLastSlash != std::string::npos) p = p.substr(pLastSlash + 1);
                
                if (p == texFile) {
                    outMaterialMap[currentMtlName] = static_cast<float>(i);
                    std::cout << "MTL: Material '" << currentMtlName << "' mapped to Texture Index " << i << " (" << texFile << ")" << std::endl;
                    break;
                }
            }
            if (outMaterialMap.find(currentMtlName) == outMaterialMap.end()) {
                std::cerr << "MTL ERROR: Texture file '" << texFile << "' not found in texturePaths array!" << std::endl;
            }
        }
    }
}