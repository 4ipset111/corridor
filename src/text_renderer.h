#ifndef __TEXT_RENDERER_H__
#define __TEXT_RENDERER_H__

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <map>
#include <string>
#include "shader.h"

struct Character {
    unsigned int TextureID; 
    glm::ivec2   Size;      
    glm::ivec2   Bearing;   
    unsigned int Advance;   
};

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();
    bool load(const std::string& fontPath, unsigned int fontSize);
    void renderText(Shader& s, const std::string& text, float x, float y, float scale, glm::vec3 color);
    float getTextWidth(const std::string& text, float scale);

private:
    std::map<char, Character> characters;
    unsigned int VAO, VBO;
};

#endif // __TEXT_RENDERER_H__