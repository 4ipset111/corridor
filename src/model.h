#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include <string>
#include <map>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Model {
public:
    Model(const std::string& path, const std::vector<std::string>& texturePaths);
    ~Model();
    void render() const;

private:
    GLuint VAO, VBO;
    GLsizei vertexCount;
    bool loadOBJ(const std::string& path, const std::vector<std::string>& texturePaths, std::vector<float>& outVertices);
    void loadMTL(const std::string& mtlPath, const std::vector<std::string>& texturePaths, std::map<std::string, float>& outMaterialMap);
};

#endif