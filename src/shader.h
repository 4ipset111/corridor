#ifndef __SHADER_H__
#define __SHADER_H__

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader
{
public:
    GLuint programID;

    Shader(const char* vertexSource, const char* fragmentSource);
    ~Shader();

    void use() const;
    void setInt(const char* name, int value) const;
    void setFloat(const char* name, float value) const;
    void setVec3(const char* name, const glm::vec3& value) const;
    void setMatrix4fv(const char* name, const glm::mat4& mat) const;
    
private:
    GLuint compileShader(const char* source, GLenum type) const;
};

#endif // __SHADER_H__
